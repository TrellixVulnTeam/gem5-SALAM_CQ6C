#include "../lenet5_hw_defines.h"

#define InputIdx3D(i,j,k) (conv0InDim*conv0InDim*(k) + conv0InDim*(j) + i)
#define KIdx3D(i,j,k) (conv0KSize*conv0KSize*(k) + conv0KSize*(i) + j)
#define OutIdx3D(i,j,k) (conv0OutDim*conv0OutDim*(k) + conv0OutDim*(j-conv0KSize/2) + i-conv0KSize/2)

void conv0() {
    uint8_t* convInput = (uint8_t*)Conv0Input;
    uint8_t* kernel = (uint8_t*)Conv0Weights;
    uint8_t* convOut = (uint8_t*)Conv0Output;
    uint8_t* convLUT = (uint8_t*)Conv0LUT;
    
    int i, j, k, l, m, n;
    #pragma clang loop unroll(disable)
    for (n = 0; n < conv0OutChan; n++){
        #pragma clang loop unroll_count(conv0UnrollFactor)
        for (k = 0; k < conv0InChan; k++){
            #pragma clang loop unroll(disable)
            for ( j = 0; j < conv0InDim; j++) {
                #pragma clang loop unroll(disable)
                for ( i = 0; i < conv0InDim; i++) {
                    if(!(i-conv0KSize/2<0 || j-conv0KSize/2<0
                        || i+conv0KSize/2>=conv0InDim
                        || j+conv0KSize/2 >= conv0InDim)){
                        int sum = 0;
                        #pragma clang loop unroll(disable)
                        for ( m = -conv0KSize/2; m < conv0KSize/2; m++) {
                            #pragma clang loop unroll(disable)
                            for ( l = -conv0KSize/2; l < conv0KSize/2; l++) {
                                sum += convInput[InputIdx3D(i+l, j+m, k)]
                                * kernel[KIdx3D(l+conv0KSize/2, m+conv0KSize/2, n)];
                            }
                        }
                        convOut[OutIdx3D(i,j,n)] += sum;
                    }
                }
            }
        }
    }
    // Apply the activation function
    #pragma clang loop unroll(disable)
    for (n = 0; n < conv0OutChan; n++){
        #pragma clang loop unroll(disable)
        for ( j = 0; j < conv0InDim; j++) {
            #pragma clang loop unroll(disable)
            for ( i = 0; i < conv0InDim; i++) {
                convOut[OutIdx3D(i,j,n)] *= convLUT[0];
            }
        }
    }
}