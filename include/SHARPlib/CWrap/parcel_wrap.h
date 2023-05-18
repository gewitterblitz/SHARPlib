#ifndef __SHARP_PARCEL_WRAP_H__
#define __SHARP_PARCEL_WRAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <SHARPlib/CWrap/profile_wrap.h>

struct sharp_Parcel {
    void *obj;
};

typedef struct sharp_Parcel sharp_Parcel_t;

sharp_Parcel_t* sharp_Parcel_create();
void sharp_Parcel_delete(sharp_Parcel_t* pcl);

float sharp_Parcel_get_pres(sharp_Parcel_t* pcl);
float sharp_Parcel_get_tmpc(sharp_Parcel_t* pcl);
float sharp_Parcel_get_dwpc(sharp_Parcel_t* pcl);
float sharp_Parcel_get_lcl_pres(sharp_Parcel_t* pcl);
float sharp_Parcel_get_lfc_pres(sharp_Parcel_t* pcl);
float sharp_Parcel_get_el_pres(sharp_Parcel_t* pcl);
float sharp_Parcel_get_cape(sharp_Parcel_t* pcl);
float sharp_Parcel_get_cinh(sharp_Parcel_t* pcl);
int sharp_Parcel_get_lpl(sharp_Parcel_t* pcl);

void sharp_define_parcel(sharp_Profile_t* prof, sharp_Parcel_t* pcl,
                         int source);

void sharp_define_custom_parcel(sharp_Parcel_t* pcl, float pres, float tmpc,
                                float dwpc);

void sharp_lift_parcel_wobf(sharp_Profile_t* prof, sharp_Parcel_t* pcl);

void sharp_find_lfc_el(sharp_Parcel_t* pcl, const float* pres,
                       const float* hght, const float* buoy, int NZ);

void sharp_cape_cinh(sharp_Profile_t* prof, sharp_Parcel_t* pcl);

void sharp_parcel_wobf(sharp_Profile_t* prof, sharp_Parcel_t* pcl);

#ifdef __cplusplus
}
#endif

#endif // __SHARP_PARCEL_WRAP_H__
