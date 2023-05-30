
/**
 * \file
 * \brief Routines used for parcel lifting and integration
 * \author
 *   Kelton Halbert                  \n
 *   Email: kelton.halbert@noaa.gov  \n
 *   License: Apache 2.0             \n
 * \date   2022-11-09
 *
 * Written for the NWS Storm Predidiction Center \n
 * Based on NSHARP routines originally written by
 * John Hart and Rich Thompson at SPC.
 */
#include <SHARPlib/constants.h>
#include <SHARPlib/interp.h>
#include <SHARPlib/layer.h>
#include <SHARPlib/parcel.h>
#include <SHARPlib/profile.h>
#include <SHARPlib/thermo.h>

#include <iostream>

namespace sharp {

/**
 * \author Kelton Halbert - NWS Storm Prediction Center/OU-CIWRO
 *
 * \brief Sets the parcel initial values to the surface of the profile
 *
 * \param prof
 * \param pcl
 *
 */
void _sfc(const float pressure[], const float temperature[],
          const float dewpoint[], const int N, Parcel& pcl) noexcept {
    pcl.pres = pressure[0];
    pcl.tmpk = temperature[0];
    pcl.dwpk = dewpoint[0];
}

/**
 * \author Kelton Halbert - NWS Storm Prediction Center/OU-CIWRO
 *
 * \brief Sets the parcel initial values to the most unstable parcel level
 *
 * \param prof
 * \param pcl
 *
 */
void _mu(const float pressure[], const float temperature[],
         const float dewpoint[], const float thetae[], const int N,
         Parcel& pcl) noexcept {
    // Search for the most unstable parcel in the bottom
    // 400 hPa of the profile
    static constexpr float mu_depth = 40000.0f; // 400 hPa in Pa
    PressureLayer mu_layer(pressure[0], pressure[0] - mu_depth);

    // layer_max returns the max, and will set the pressure
    // of the max via a pointer to a float.
    layer_max(mu_layer, pressure, thetae, N, &(pcl.pres));
    pcl.tmpk = interp_pressure(pcl.pres, pressure, temperature, N);
    pcl.dwpk = interp_pressure(pcl.pres, pressure, dewpoint, N);
}

/**
 * \author Kelton Halbert - NWS Storm Prediction Center/OU-CIWRO
 *
 * \brief Sets the parcel initial values to the bottom 100mb mixed layer
 *
 * \param prof
 * \param pcl
 *
 */
void _ml(const float pressure[], const float theta_arr[],
         const float wv_mixratio[], const int N, Parcel& pcl) noexcept {
    static constexpr float ml_depth = 10000.0; // 100 hPa in Pa
	const float sfcpres = pressure[0];
    PressureLayer mix_layer(sfcpres, sfcpres - ml_depth);

    // get the mean attributes of the lowest 100 hPa
    const float mean_mixr =
        layer_mean(mix_layer, pressure, wv_mixratio, N);
    const float mean_thta =
        layer_mean(mix_layer, pressure, theta_arr, N);

    // set the parcel attributes
    pcl.pres = sfcpres;
    pcl.tmpk = theta(THETA_REF_PRESSURE, mean_thta, sfcpres);
    pcl.dwpk = temperature_at_mixratio(mean_mixr, sfcpres);
}

void define_parcel(const float pressure[], const float temperature[],
                   const float dewpoint[],
                   const float wv_mixratio[], const float theta_arr[],
                   const float thetae[], const int N, Parcel& pcl,
                   LPL source) noexcept {
    pcl.source = source;

    if (source == LPL::SFC) {
        _sfc(pressure, temperature, dewpoint, N, pcl);
        return;
    } else if (source == LPL::FCST) {
        // TO-DO: Write the forecast_surface routine
        return;
    } else if (source == LPL::MU) {
        _mu(pressure, temperature, dewpoint, thetae, N, pcl);
        return;
    } else if (source == LPL::ML) {
        _ml(pressure, theta_arr, wv_mixratio, N, pcl);
        return;
    } else if (source == LPL::EIL) {
        // TO-DO: Write the EIL routine
        return;
    } else if (source == LPL::USR) {
        // do nothing - its already been set!
        return;
    } else {
        // TO-DO: probably should raise an error or something
        return;
    }
}

void find_lfc_el(Parcel* pcl, const float pres_arr[], const float hght_arr[],
                 const float buoy_arr[], const int NZ) noexcept {
    PressureLayer sat_lyr = {pcl->lcl_pressure, pres_arr[NZ - 1]};
    LayerIndex lyr_idx = get_layer_index(sat_lyr, pres_arr, NZ);

    float lyr_bot = 0.0;
    float pos_buoy = 0.0;
    float pos_buoy_last = 0.0;
    float pbot = sat_lyr.bottom;
    float buoy_bot = interp_pressure(sat_lyr.bottom, pres_arr, buoy_arr, NZ);
    float hbot = interp_pressure(sat_lyr.bottom, pres_arr, hght_arr, NZ);
    // set the LFC pressure to the LCL if the buoyancy is positive
    float lfc_pres = (buoy_bot > 0) ? sat_lyr.bottom : MISSING;
    float eql_pres = MISSING;
    float lfc_pres_last = MISSING;
    float eql_pres_last = MISSING;

    for (int k = lyr_idx.kbot; k < lyr_idx.ktop + 1; ++k) {
#ifndef NO_QC
        if (buoy_arr[k] == MISSING) continue;
#endif
        const float ptop = pres_arr[k];
        const float htop = hght_arr[k];
        const float buoy_top = buoy_arr[k];
        const float lyr_top = (buoy_top + buoy_bot) / 2.0f;
        // LFC condition
        if ((lyr_bot <= 0) && (lyr_top > 0)) {
            if (lfc_pres != MISSING) {
                pos_buoy_last = pos_buoy;
                lfc_pres_last = lfc_pres;
                eql_pres_last = eql_pres;
                pos_buoy = 0.0;
            }
            for (lfc_pres = pbot - 500; lfc_pres > ptop + 500;
                 lfc_pres -= 100.0) {
                const float buoy =
                    interp_pressure(lfc_pres, pres_arr, buoy_arr, NZ);
                if (buoy > 0) break;
            }
        }

        // keep track of buoyancy so that we pick the max CAPE layer
        const float condition = ((lfc_pres != MISSING) & (lyr_top > 0.0));
        pos_buoy += condition * (htop - hbot) * lyr_top;
        // EL condition
        if ((lfc_pres != MISSING) && ((lyr_bot >= 0) && (lyr_top < 0))) {
            for (eql_pres = pbot - 500; eql_pres > ptop + 500;
                 eql_pres -= 100.0) {
                const float buoy =
                    interp_pressure(eql_pres, pres_arr, buoy_arr, NZ);
                if (buoy < 0) break;
            }
            if (pos_buoy_last > pos_buoy) {
                lfc_pres = lfc_pres_last;
                eql_pres = eql_pres_last;
                pos_buoy = pos_buoy_last;
            }
        }
        // If there is no EL, just use the last available level
        if ((k == NZ - 1) && (lyr_top > 0)) eql_pres = pres_arr[NZ - 1];
        // set loop variables
        pbot = ptop;
        hbot = htop;
        buoy_bot = buoy_top;
        lyr_bot = lyr_top;
    }
    pcl->lfc_pressure = lfc_pres;
    pcl->eql_pressure = eql_pres;
}

//void cape_cinh(Profile* prof, Parcel* pcl) noexcept {
void cape_cinh(const float pressure[], const float height[],
               const float buoyancy_arr[], const int N, Parcel* pcl) noexcept {
    find_lfc_el(pcl, pressure, height, buoyancy_arr, N);
    if ((pcl->lfc_pressure != MISSING) && (pcl->eql_pressure != MISSING)) {
		PressureLayer lfc_el = {pcl->lfc_pressure, pcl->eql_pressure};
		PressureLayer lpl_lfc = {pcl->pres, pcl->lfc_pressure};
        HeightLayer lfc_el_ht =
            pressure_layer_to_height(lfc_el, pressure, height, N);
        HeightLayer lpl_lfc_ht =
            pressure_layer_to_height(lpl_lfc, pressure, height, N);

        pcl->cinh =
            integrate_layer_trapz(lpl_lfc_ht, buoyancy_arr, height, N, -1);
        pcl->cape =
            integrate_layer_trapz(lfc_el_ht, buoyancy_arr, height, N, 1);
    }
}

void parcel_wobf(Profile* prof, Parcel* pcl) noexcept {
    static constexpr lifter_wobus lifter;
    lift_parcel(lifter, prof->pres, prof->vtmp, prof->buoyancy, prof->NZ, pcl);
    cape_cinh(prof->pres, prof->hght, prof->buoyancy, prof->NZ, pcl);
}

}  // end namespace sharp

namespace sharp::exper {}  // end namespace sharp::exper
