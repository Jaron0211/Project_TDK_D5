
//尋跡(左右)參數
extern float LINE_DETECT_VALUE_P = 4.3;
extern float LINE_DETECT_VALUE_I = 0.1;
extern float LINE_DETECT_VALUE_D = 1;

//尋跡(水平旋轉)參數
extern float LINE_DIRECTION_VALUE = 1.5;

//停等區參數
extern float X_CCC_DETECT_VALUE_P = 0.8;
extern float X_CCC_DETECT_VALUE_D = 0.8;
extern float Y_CCC_DETECT_VALUE_P = 1.6;
extern float Y_CCC_DETECT_VALUE_D = 1;

//直角交合點範圍參數
extern int COMPARE_DIS = 5;
//下方PIXY2偵測總線數量
extern int max_num = 5;

//尋跡防呆機制參數(越大越滑順，但是反應越慢)(0~20)
extern int averge_component = 8;

extern int detect_angle = 15;
extern int detect_distance = 20;
