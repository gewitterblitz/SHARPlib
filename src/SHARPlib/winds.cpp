/**
 * \file
 * \brief Routines used to compute kinematic attributes and indices of vertical sounding profiles
 * \author  
 *   Kelton Halbert                  \n
 *   Email: kelton.halbert@noaa.gov  \n
 *   License: Apache 2.0             \n
 * \date   2022-10-13
 *
 * Written for the NWS Storm Predidiction Center \n
 * Based on NSHARP routines originally written by
 * John Hart and Rich Thompson at SPC. 
 */
#include <cmath>

#include <SHARPlib/constants.h>
#include <SHARPlib/interp.h>
#include <SHARPlib/utils.h>
#include <SHARPlib/winds.h>

namespace sharp {


float u_component(float wind_speed, float wind_direction) {
#ifndef NO_QC
    if ((wind_direction == MISSING) || (wind_speed == MISSING))
        return MISSING;
#endif
    wind_direction *= (PI / 180.0);
    return -1.0 * wind_speed * std::sin(wind_direction);
}


float v_component(float wind_speed, float wind_direction) {
#ifndef NO_QC
    if ((wind_direction == MISSING) || (wind_speed == MISSING))
        return MISSING;
#endif
    wind_direction *= (PI / 180.0);
    return -1.0 * wind_speed * std::cos(wind_direction);
}


float vector_angle(float u_comp, float v_comp) {
#ifndef NO_QC
    if ((u_comp == MISSING) || (v_comp == MISSING))
        return MISSING;
#endif
    if ((u_comp == 0) && (v_comp == 0))
        return 0;

    float wind_direction = std::atan2( -1*u_comp, -1*v_comp) * (180.0 / PI);
    if (wind_direction < 0) wind_direction += 360.0;
    if (wind_direction < TOL) wind_direction = 0;
    return wind_direction; 
}


float vector_magnitude(float u_comp, float v_comp) {
#ifndef NO_QC
    if ((u_comp == MISSING) || (v_comp == MISSING))
        return MISSING;
#endif
    return std::sqrt((u_comp * u_comp) + (v_comp * v_comp));
}


float vector_magnitude_precise(float u_comp, float v_comp) {
#ifndef NO_QC
    if ((u_comp == MISSING) || (v_comp == MISSING))
        return MISSING;
#endif
    return std::hypot(u_comp, v_comp);
}


WindVector components_to_vector(float u_comp, float v_comp) {
    float wind_speed = vector_magnitude(u_comp, v_comp);
    float wind_direction = vector_angle(u_comp, v_comp);

    return {wind_speed, wind_direction};
}


WindVector components_to_vector(WindComponents comp) {
    float wind_speed = vector_magnitude(comp.u, comp.v);
    float wind_direction = vector_angle(comp.u, comp.v);

    return {wind_speed, wind_direction};
}


WindComponents vector_to_components(float wind_speed, float wind_direction) {
#ifndef NO_QC
    if ((wind_direction == MISSING) || (wind_speed == MISSING))
        return {MISSING, MISSING}; 
#endif
    wind_direction *= (PI / 180.0);
    float u_comp = -1.0 * wind_speed * std::sin(wind_direction);
    float v_comp = -1.0 * wind_speed * std::cos(wind_direction);

    return {u_comp, v_comp};
}


WindComponents vector_to_components(WindVector vect) {
#ifndef NO_QC
    if ((vect.speed == MISSING) || (vect.direction == MISSING))
        return {MISSING, MISSING}; 
#endif
    float wind_direction = vect.direction * (PI / 180.0);
    float u_comp = -1.0 * vect.speed * std::sin(wind_direction);
    float v_comp = -1.0 * vect.speed * std::cos(wind_direction);

    return {u_comp, v_comp};
}


WindComponents mean_wind(PressureLayer layer, const float* pres,
                         const float* u_wind, const float* v_wind, 
                         int num_levs) {
#ifndef NO_QC
    if ((layer.pbot == MISSING) || (layer.ptop == MISSING))
        return {MISSING, MISSING};
#endif

    // bounds checking our layer
    // requests to the available data
    // -- since PressureLayer is passed
    // by value, this does not modify 
    // the original object
    if (layer.pbot > pres[0])
        layer.pbot = pres[0];

    if (layer.ptop < pres[num_levs-1])
        layer.ptop = pres[num_levs-1];


    float pr_lvl = layer.pbot;
    float u_sum = 0;
    float v_sum = 0;
    float weight = 0;
    while(pr_lvl >= layer.ptop) {
        u_sum += interp_pressure(pr_lvl, pres, u_wind, num_levs) * pr_lvl; 
        v_sum += interp_pressure(pr_lvl, pres, v_wind, num_levs) * pr_lvl; 
        weight += pr_lvl;
        pr_lvl -= layer.dp;
    }

    float mean_u = u_sum / weight;
    float mean_v = v_sum / weight;

    return {mean_u, mean_v}; 
}


WindComponents mean_wind_npw(PressureLayer layer, const float* pres,
                             const float* u_wind, const float* v_wind, 
                             int num_levs) {
#ifndef NO_QC
    if ((layer.pbot == MISSING) || (layer.ptop == MISSING))
        return {MISSING, MISSING};
#endif

    // bounds checking our layer
    // requests to the available data
    // -- since PressureLayer is passed
    // by value, this does not modify 
    // the original object
    if (layer.pbot > pres[0])
        layer.pbot = pres[0];

    if (layer.ptop < pres[num_levs-1])
        layer.ptop = pres[num_levs-1];


    float pr_lvl = layer.pbot;
    float u_sum = 0;
    float v_sum = 0;
    float weight = 0;
    while(pr_lvl >= layer.ptop) {
        u_sum += interp_pressure(pr_lvl, pres, u_wind, num_levs);
        v_sum += interp_pressure(pr_lvl, pres, v_wind, num_levs);
        weight += 1; 
        pr_lvl -= layer.dp;
    }

    float mean_u = u_sum / weight;
    float mean_v = v_sum / weight;

    return {mean_u, mean_v}; 

}


WindComponents wind_shear(PressureLayer layer, const float* pres, 
                          const float* u_wind, const float* v_wind, 
                          int num_levs) {
#ifndef NO_QC
    if ((layer.pbot == MISSING) || (layer.ptop == MISSING))
        return {MISSING, MISSING};
#endif

    // bounds checking our layer
    // requests to the available data
    // -- since PressureLayer is passed
    // by value, this does not modify 
    // the original object
    if (layer.pbot > pres[0])
        layer.pbot = pres[0];
    if (layer.ptop < pres[num_levs-1])
        layer.ptop = pres[num_levs-1];

    float u_bot = interp_pressure(layer.pbot, pres, u_wind, num_levs);
    float u_top = interp_pressure(layer.ptop, pres, u_wind, num_levs);

    float v_bot = interp_pressure(layer.pbot, pres, v_wind, num_levs);
    float v_top = interp_pressure(layer.ptop, pres, v_wind, num_levs);

#ifndef NO_QC
    if ((u_bot == MISSING) || (v_bot == MISSING) ||
        (u_top == MISSING) || (v_top == MISSING)) {

        return {MISSING, MISSING}; 
    }
#endif

    return {u_top - u_bot, v_top - v_bot};
}


WindComponents wind_shear(HeightLayer layer_agl, const float* height, 
                          const float* u_wind, const float* v_wind, 
                          int num_levs) {
#ifndef NO_QC
    if ((layer_agl.zbot == MISSING) || (layer_agl.ztop == MISSING))
        return {MISSING, MISSING};
#endif

    // bounds checking our layer
    // requests to the available data
    // -- since PressureLayer is passed
    // by value, this does not modify 
    // the original object
    if (layer_agl.zbot > height[0])
        layer_agl.zbot = height[0];
    if (layer_agl.ztop < height[num_levs-1])
        layer_agl.ztop = height[num_levs-1];

    float u_bot = interp_height(layer_agl.zbot, height, u_wind, num_levs);
    float u_top = interp_height(layer_agl.ztop, height, u_wind, num_levs);

    float v_bot = interp_height(layer_agl.zbot, height, v_wind, num_levs);
    float v_top = interp_height(layer_agl.ztop, height, v_wind, num_levs);

#ifndef NO_QC
    if ((u_bot == MISSING) || (v_bot == MISSING) ||
        (u_top == MISSING) || (v_top == MISSING)) {

        return {MISSING, MISSING}; 
    }
#endif

    return {u_top - u_bot, v_top - v_bot};
}


// TO-DO: Right now it is unclear whether the base usings of
// u_wind and v_wind should be knots or m/s. Need to clarify
// design here and update documentation accordingly. 
float helicity(HeightLayer layer_agl, WindComponents storm_motion,
               const float* height, const float* u_wind, 
               const float* v_wind, int num_levs) {
#ifndef NO_QC
    if ((storm_motion.u == MISSING) || (storm_motion.v == MISSING)) {
        return MISSING;
    }
    if ((layer_agl.zbot == MISSING) || (layer_agl.ztop == MISSING)) {
        return MISSING;
    }
#endif

    // get the height in MSL by adding the surface height
    // -- modifying in this function does not modify
    // original layer struct since it is passed by value
    layer_agl.zbot += height[0];
    layer_agl.ztop += height[0];

    // Get the vertical array indices corresponding to our layer,
    // while also bounds checking our search. Indices exclude the 
    // top and bottom layers that will be interpolated.
    LayerIndex layer_idx = get_layer_index(layer_agl, height, num_levs);

    // Get the interpolated bottom of the layer to integrate
    // and convert to storm-relative winds
    float sru_bot = interp_height(layer_agl.zbot, height, u_wind, num_levs);
    float srv_bot = interp_height(layer_agl.zbot, height, v_wind, num_levs);
    sru_bot -= storm_motion.u;
    srv_bot -= storm_motion.v;

    // will get set in first loop iter
    float sru_top;
    float srv_top;
    float layer_helicity = 0.0;
    for (int k = layer_idx.kbot; k <= layer_idx.ktop; k++) {
#ifndef NO_QC
        if ((u_wind[k] == MISSING) || (v_wind[k] == MISSING)) {
            continue;
        }
#endif
        // top of layer storm relative winds
        sru_top = u_wind[k] - storm_motion.u;
        srv_top = v_wind[k] - storm_motion.v;

        // integrate layer
        layer_helicity += (sru_top * srv_bot) - (sru_bot * srv_top); 
        // set the top to be the bottom
        // of the next layer
        sru_bot = sru_top;
        srv_bot = srv_top;
    }

    // Get the interpolated top of the layer to integrate
    // and convert to storm-relative winds
    sru_top = interp_height(layer_agl.ztop, height, u_wind, num_levs);
    srv_top = interp_height(layer_agl.ztop, height, v_wind, num_levs);
    sru_top -= storm_motion.u;
    srv_top -= storm_motion.v;
    
    // integrate the final layer
    layer_helicity = (sru_top * srv_bot) - (sru_bot * srv_top); 

    return layer_helicity; 
}

WindComponents storm_motion_bunkers_np(const float *pressure,
        const float *height, const float *u_wind, const float *v_wind,
        int num_levs, bool leftMover) {
    // derived storm deviation from the mean wind
    // given in m/s 
    float deviation = 7.5;

    // get the pressure values of the AGL heights required
    float pressure_sfc = pressure[0];
    float pressure_500m = interp_height(height[0]+500.0, height, 
                                        pressure, num_levs);
    float pressure_5500m = interp_height(height[0]+5500.0, height, 
                                         pressure, num_levs);
    float pressure_6000m = interp_height(height[0]+6000.0, height, 
                                         pressure, num_levs);

    // set up the layers
    PressureLayer layer_lo = {pressure_sfc, pressure_500m};
    PressureLayer layer_hi = {pressure_5500m, pressure_6000m};
    PressureLayer layer_tot = {pressure_sfc, pressure_6000m};


    // get the mean wind of these two layers
    WindComponents mean_wind_0_500m = mean_wind(layer_lo, pressure, 
                                          u_wind, v_wind,num_levs); 

    WindComponents mean_wind_5500_6000m = mean_wind(layer_hi, pressure, 
                                             u_wind, v_wind, num_levs); 

    WindComponents mean_wind_0_6000m = mean_wind_npw(layer_tot, 
                           pressure, u_wind, v_wind, num_levs); 

    float shear_u = mean_wind_5500_6000m.u - mean_wind_0_500m.u;
    float shear_v = mean_wind_5500_6000m.v - mean_wind_0_500m.v;
    float mag = vector_magnitude(shear_u, shear_v);

    float storm_u = MISSING;
    float storm_v = MISSING;
    if (leftMover) {
        storm_u = mean_wind_0_6000m.u - ( (deviation / mag) * shear_v); 
        storm_v = mean_wind_0_6000m.v + ( (deviation / mag) * shear_u); 
    }

    else {
        storm_u = mean_wind_0_6000m.u + ( (deviation / mag) * shear_v); 
        storm_v = mean_wind_0_6000m.v - ( (deviation / mag) * shear_u); 
    }

    return {storm_u, storm_v};
}

} // end namespace sharp


namespace sharp::exper {


} // end namespace sharp::exper


