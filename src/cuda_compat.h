#pragma once
#ifdef __CUDACC__
#define HD __host__ __device__
#define DEVICE __device__
#else
#define HD
#define DEVICE
#endif