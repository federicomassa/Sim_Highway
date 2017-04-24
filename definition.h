
#ifndef DEFINITION_H
#define DEFINITION_H

/* output dir (make sure NOT to add the trailing slash) */
#define OUTPUT_DIR "/home/federico-asus/Simulator/Sim_Highway/Output"
#define LOG_FILE_NAME OUTPUT_DIR "/Simulator.log"
#define OUTPUT(f) (OUTPUT_DIR "/" #f)

/* input dir (make sure NOT to add the trailing slash) */
#define INPUT_DIR "/home/federico-asus/Simulator/Sim_Highway/Input"
#define INPUT(f) (INPUT_DIR "/" #f)

/* area space dimension */
#define Q_DIM 2

// Image output parameters:
/* MAX_LANE: see also rules.h */
#define LANES 2
#define SCALE 50
#define VEHICLE_IMG_W 61
#define VEHICLE_IMG_H 36
#define VEHICLE_TXT_SIZE 0.27
#define VEHICLE_TXT_SCALE 61
#define FRAME_W 900
#define FRAME_H (17 + SCALE * LANES)
#define Y_OFFSET (FRAME_H - 9)
#define FPS 20
#define FONT_NAME INPUT_DIR "/times.ttf"
#define AREA_OPACITY 20

#endif
