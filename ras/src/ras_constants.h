#ifndef __RAS_CONSTANTS_H__
#define __RAS_CONSTANTS_H__

#define __USE_THREADS__

const bool   C_COMPUTE_CDA_DEFAULT = true;
const bool   C_COMPUTE_CRR_DEFAULT = false;

const int    C_SLEEP_TIME       = 25;   // milliseconds
const int    C_MAX_SIMPSON_RULE = 15; 
const int    C_MIN_SIMPSON_RULE =  1;  
const double C_EPS_SIMPSON_RULE = 5E-12;

const double C_WHEEL_TORQUE_ETA = 1.0;
const double C_CRANK_TORQUE_ETA = 0.98;

const char   C_RAS_INPUT_IP_ADDRESS[]  = "127.0.0.1";
const char   C_RAS_OUTPUT_IP_ADDRESS[] = "127.0.0.1";
const int    C_RAS_INPUT_PORT_NUMBER   = 4005;
const int    C_RAS_OUTPUT_PORT_NUMBER  = 4006;

#endif // __RAS_CONSTANTS_H__

