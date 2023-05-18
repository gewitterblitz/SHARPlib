#include <SHARPlib/CWrap/thermo_wrap.h>
#include <SHARPlib/CWrap/layer_wrap.h>
#include <SHARPlib/constants.h>
#include <SHARPlib/thermo.h>

float sharp_wobf(float temperature) {
    return sharp::wobf(temperature);
}

float sharp_vapor_pressure(float temperature) {
    return sharp::vapor_pressure(temperature);
}

float sharp_lcl_temperature(float temperature, float dewpoint) {
    return sharp::lcl_temperature(temperature, dewpoint);
}

float sharp_temperature_at_mixratio(float wv_mixratio, float pressure) {
    return sharp::temperature_at_mixratio(wv_mixratio, pressure);
}

float sharp_theta_level(float potential_temperature, float temperature) {
    return sharp::theta_level(potential_temperature, temperature);
}

float sharp_theta(float pressure, float temperature, float ref_pressure) {
    return sharp::theta(pressure, temperature, ref_pressure);
}

float sharp_mixratio(float pressure, float temperature) {
    return sharp::mixratio(pressure, temperature);
}

float sharp_virtual_temperature(float pressure, float temperature,
                                float dewpoint) {
    return sharp::virtual_temperature(pressure, temperature, dewpoint);
}

float sharp_saturated_lift(float pressure, float theta_sat) {
    return sharp::saturated_lift(pressure, theta_sat);
}

float sharp_wetlift(float pressure, float temperature, float lifted_pressure) {
    return sharp::wetlift(pressure, temperature, lifted_pressure);
}

void sharp_drylift(float pressure, float temperature, float dewpoint,
                   float* pressure_at_lcl, float* temperature_at_lcl) {
    if ((pressure_at_lcl == NULL) || (temperature_at_lcl == NULL)) return;
    sharp::drylift(pressure, temperature, dewpoint, *pressure_at_lcl,
                   *temperature_at_lcl);
}

float sharp_lifted(float pressure, float temperature, float dewpoint,
                   float lifted_pressure) {
    return sharp::lifted(pressure, temperature, dewpoint, lifted_pressure);
}

float sharp_wetbulb(float pressure, float temperature, float dewpoint) {
    return sharp::wetbulb(pressure, temperature, dewpoint);
}

float sharp_theta_wetbulb(float pressure, float temperature, float dewpoint) {
    return sharp::theta_wetbulb(pressure, temperature, dewpoint);
}

float sharp_thetae(float pressure, float temperature, float dewpoint) {
    return sharp::thetae(pressure, temperature, dewpoint);
}

float sharp_HeightLayer_lapse_rate(sharp_HeightLayer_t* layer_agl,
                                   const float* height,
                                   const float* temperature, int NZ) {
    if (layer_agl == NULL) return sharp::MISSING;
    sharp::HeightLayer* h = static_cast<sharp::HeightLayer*>(layer_agl->obj);
    return sharp::lapse_rate(*h, height, temperature, NZ);
}

float sharp_PressureLayer_lapse_rate(sharp_PressureLayer_t* layer,
                                     const float* pressure, const float* height,
                                     const float* temperature, int NZ) {
    if (layer == NULL) return sharp::MISSING;
    sharp::PressureLayer* p = static_cast<sharp::PressureLayer*>(layer->obj);
    return sharp::lapse_rate(*p, pressure, height, temperature, NZ);
}

float sharp_buoyancy(float pcl_temperature, float env_temperature) {
    return sharp::buoyancy(pcl_temperature, env_temperature);
}

float sharp_moist_static_energy(float height_agl, float temperature,
                                float specific_humidity) {
    return sharp::moist_static_energy(height_agl, temperature, specific_humidity);
}

