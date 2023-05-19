#include <SHARPlib/CWrap/interp_wrap.h>
#include <SHARPlib/CWrap/layer_wrap.h>
#include <SHARPlib/CWrap/params_wrap.h>
#include <SHARPlib/CWrap/parcel_wrap.h>
#include <SHARPlib/CWrap/profile_wrap.h>
#include <SHARPlib/CWrap/thermo_wrap.h>
#include <SHARPlib/CWrap/winds_wrap.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * Reads the sounding file and counts the number of
 * lines that correspond to data levels and returns
 * that number. This is so that arrays can be sized
 * properly for the data available.
 */
int get_num_levs_in_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    char line[256];  // Assuming a maximum line length of 255 characters
    int i;

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 0;  // Exit the function
    }
    while (fgets(line, 256, file) != 0) {
        if (strstr(line, "%RAW%") != 0) {
            i = 0;
            while ((fgets(line, 256, file) != 0) &&
                   (strstr(line, "%END%") == 0)) {
                ++i;
            }
        }
    }
    fclose(file);
    return i;
}

/**
 * Reads the sounding file and fills the arrays with the data. These
 * arrays should have already been allocated to the appropriate size
 * before entering this routine.
 */
void read_snd_file(const char* filename, float* pres, float* hght, float* tmpc,
                   float* dwpc, float* wspd, float* wdir, float* omeg) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;  // Exit the function
    }

    char line[256];  // Assuming a maximum line length of 255 characters
    int i, j;

    while (fgets(line, 256, file) != 0) {
        if (strstr(line, "%RAW%") != 0) {
            i = 0;
            while ((fgets(line, 256, file) != 0) &&
                   (strstr(line, "%END%") == 0)) {
                j = sscanf(line, "%f,%f,%f,%f,%f,%f,%f", &(pres[i]), &(hght[i]),
                           &(tmpc[i]), &(dwpc[i]), &(wdir[i]), &(wspd[i]),
                           &(omeg[i]));
                ++i;
            }
        }
    }

    fclose(file);  // Close the file
}

/**
 * Loads the sounding data from the file into the Profile object.
 *
 * This function will call get_num_levs_in_file and read_snd_file
 * in order to allocate and fill the arrays. Then, the remaining
 * Sounding variables, such as mixing ratio and virtual temprature,
 * are computed and filled.
 */
void load_sounding(const char* filename, sharp_Profile_t* prof) {
    int NZ = get_num_levs_in_file(filename);
    printf("NZ = %d\n", NZ);

    // get the base data arrays and fill them with the data from the file
    float* pres = sharp_Profile_get_pres_ptr(prof);
    float* hght = sharp_Profile_get_hght_ptr(prof);
    float* tmpc = sharp_Profile_get_tmpc_ptr(prof);
    float* dwpc = sharp_Profile_get_dwpc_ptr(prof);
    float* wdir = sharp_Profile_get_wdir_ptr(prof);
    float* wspd = sharp_Profile_get_wspd_ptr(prof);
    float* omeg = sharp_Profile_get_vvel_ptr(prof);
    read_snd_file(filename, pres, hght, tmpc, dwpc, wspd, wdir, omeg);

    // fill the other arrays with computed data
    float* mixr = sharp_Profile_get_mixr_ptr(prof);
    float* vtmp = sharp_Profile_get_vtmp_ptr(prof);
    float* uwin = sharp_Profile_get_uwin_ptr(prof);
    float* vwin = sharp_Profile_get_vwin_ptr(prof);
    float* theta = sharp_Profile_get_theta_ptr(prof);
    float* thetae = sharp_Profile_get_thetae_ptr(prof);
    float* mse = sharp_Profile_get_mse_ptr(prof);
    for (int i = 0; i < NZ; ++i) {
        // wind speeds in these files are in knots,
        // but we want them in m/s instead
        wspd[i] = wspd[i] * 0.5144444444444444f;

        float p = pres[i];
        float h = hght[i];
        float t = tmpc[i];
        float d = dwpc[i];
        float spd = wspd[i];
        float dir = wdir[i];

        mixr[i] = sharp_mixratio(p, d);
        vtmp[i] = sharp_virtual_temperature(p, t, d);
        theta[i] = sharp_theta(p, t, 1000.0);
        thetae[i] = sharp_thetae(p, t, d);
        uwin[i] = sharp_u_component(spd, dir);
        vwin[i] = sharp_v_component(spd, dir);

        // need to convert mixing ratio from g/kg to kg/kg
        float specific_humidity =
            (1.0 - (mixr[i] / 1000.0)) * (mixr[i] / 1000.0);
        if (mixr[i] == -9999.0f) specific_humidity = -9999.0f;

        float height_agl = hght[i] - hght[0];
        mse[i] = sharp_moist_static_energy(height_agl, t + 273.15,
                                           specific_humidity);
    }
}

int main(int argc, char** argv) {
    // performance counters
    clock_t start, end;
    double cpu_time_used;

	// filename and number of vertical levels in the file
    const char* filename = "20160524_2302_EF3_37.57_-100.13_108_613967.snd";
    int NZ = get_num_levs_in_file(filename);

    // create a Profile with the required number of vertical levels
    sharp_Profile_t* prof = sharp_Profile_create(NZ, 0);

    // load the sounding from disk, and compute the remaining
    // variables needed from the base data.
    start = clock();
    load_sounding(filename, prof);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Reading and preprocessing time: %fs\n", cpu_time_used);

    // print the contents of these arrays so that we
    // can see we've successfully loaded the data!
    float* pres = sharp_Profile_get_pres_ptr(prof);
    float* hght = sharp_Profile_get_hght_ptr(prof);
    float* tmpc = sharp_Profile_get_tmpc_ptr(prof);
    float* dwpc = sharp_Profile_get_dwpc_ptr(prof);
    float* wdir = sharp_Profile_get_wdir_ptr(prof);
    float* wspd = sharp_Profile_get_wspd_ptr(prof);
    float* omeg = sharp_Profile_get_vvel_ptr(prof);
    for (int i = 0; i < NZ; ++i) {
        printf(
            "pres[%d] = %f hght[%d] = %f tmpc[%d] = %f dwpc[%d] = %f wdir[%d] "
            "= %f wspd[%d] = %f omeg[%d] = %f\n",
            i, pres[i], i, hght[i], i, tmpc[i], i, dwpc[i], i, wdir[i], i,
            wspd[i], i, omeg[i]);
    }

    start = clock();
    // lets compute CAPE and CINH for a surface based, mixed layer,
    // and most unstable parcel. First, we need to create the parcels.
    sharp_Parcel_t* sb_pcl = sharp_Parcel_create();
    sharp_Parcel_t* mu_pcl = sharp_Parcel_create();
    sharp_Parcel_t* ml_pcl = sharp_Parcel_create();

    // set parcel attributes to the Surface Based Parcel
    sharp_define_parcel(prof, sb_pcl, 1);
    // Then the Most Unstable parcel
    sharp_define_parcel(prof, mu_pcl, 3);
    // Finally the mixed layer parcel
    sharp_define_parcel(prof, ml_pcl, 4);

    // Lift the parcel to compute buoyancy, followed by
    // integrating the CAPE and CINH. The buoyancy data is stored
    // a single array, so we need to do these operations separately
    // for each parcel.
    sharp_lift_parcel_wobf(prof, sb_pcl);
    sharp_cape_cinh(prof, sb_pcl);

    // Now the MU parcel
    sharp_lift_parcel_wobf(prof, mu_pcl);
    sharp_cape_cinh(prof, mu_pcl);

    // Finally do the ML parcel
    sharp_lift_parcel_wobf(prof, ml_pcl);
    sharp_cape_cinh(prof, ml_pcl);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    float sb_cape = sharp_Parcel_get_cape(sb_pcl);
    float sb_cinh = sharp_Parcel_get_cinh(sb_pcl);

    float mu_cape = sharp_Parcel_get_cape(mu_pcl);
    float mu_cinh = sharp_Parcel_get_cinh(mu_pcl);

    float ml_cape = sharp_Parcel_get_cape(ml_pcl);
    float ml_cinh = sharp_Parcel_get_cinh(ml_pcl);

    printf("SB CAPE: %f J/kg\tSB CINH: %f J/kg\n", sb_cape, sb_cinh);
    printf("MU CAPE: %f J/kg\tMU CINH: %f J/kg\n", mu_cape, mu_cinh);
    printf("ML CAPE: %f J/kg\tML CINH: %f J/kg\n", ml_cape, ml_cinh);
    printf("Lifting 3 parcels took %fs\n", cpu_time_used);

    // We MUST clean up any objects we create,
    // otherwise we will leak memory! DON'T LEAK MEMORY!
    sharp_Profile_delete(prof);
    sharp_Parcel_delete(sb_pcl);
    sharp_Parcel_delete(mu_pcl);
    sharp_Parcel_delete(ml_pcl);
    return 0;
}
