#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned char u8;
typedef unsigned int u32;

int main(){
	const u32 row[4] = {6,13,19,26};
	const u32 column[4] = {12,16,20,21};
	struct gpiod_chip *chip = NULL;
	struct gpiod_line_settings *read_settings = NULL;
	struct gpiod_line_settings *write_settings = NULL;
	struct gpiod_line_config *read_line_cfg = NULL , *write_line_cfg = NULL;
	struct gpiod_request_config *read_req_cfg =NULL, *write_req_cfg =NULL;
	struct gpiod_line_request *read_request = NULL, *write_request = NULL;
	int outgpio[4];
	enum gpiod_line_value values[4][4];
	struct {
		u32 down;
	}key[4][4];

	chip = gpiod_chip_open("/dev/gpiochip0");
	read_settings = gpiod_line_settings_new();
	gpiod_line_settings_set_direction(read_settings,GPIOD_LINE_DIRECTION_INPUT);
	gpiod_line_settings_set_bias(read_settings,GPIOD_LINE_BIAS_PULL_UP);
	write_settings = gpiod_line_settings_new();
	gpiod_line_settings_set_direction(write_settings,GPIOD_LINE_DIRECTION_OUTPUT);
	read_line_cfg = gpiod_line_config_new();
	write_line_cfg = gpiod_line_config_new();
	for (int i=0;i<4;i++){
		gpiod_line_config_add_line_settings(write_line_cfg,&row[i],1,write_settings);
		gpiod_line_config_add_line_settings(read_line_cfg,&column[i],1,read_settings);
	}
	read_req_cfg = gpiod_request_config_new();
	write_req_cfg = gpiod_request_config_new();
	gpiod_request_config_set_consumer(read_req_cfg, "keyboard-read");
	gpiod_request_config_set_consumer(write_req_cfg, "keyboard-write");
	read_request = gpiod_chip_request_lines(chip,read_req_cfg,read_line_cfg);
	write_request = gpiod_chip_request_lines(chip,write_req_cfg,write_line_cfg);

	while(1){
		for(int i=0;i<4;i++){
			outgpio[0] = outgpio[1] = outgpio[2] = outgpio[3] = 1,outgpio[i] = 0;
			gpiod_line_request_set_values(write_request,outgpio);
			for (int j=0;j<4;j++){
				gpiod_line_request_get_values(read_request,*(values+(i<<2)));
				if(!values[i][j] && key[i][j].down<8388607){
					++key[i][j].down;
				}
				else if(values[i][j] && key[i][j].down){
					key[i][j].down = 0;
				}
				printf("%dX%d:%05d",0,1 ,key[0][1].down );
				printf("\r");
			}
		}
	}
    if (read_request) gpiod_line_request_release(read_request);
    if (read_req_cfg) gpiod_request_config_free(read_req_cfg);
    if (read_line_cfg) gpiod_line_config_free(read_line_cfg);
    if (write_request) gpiod_line_request_release(write_request);
    if (write_req_cfg) gpiod_request_config_free(write_req_cfg);
    if (write_line_cfg) gpiod_line_config_free(write_line_cfg);
    if (read_settings) gpiod_line_settings_free(read_settings);
    if (write_settings) gpiod_line_settings_free(write_settings);
    if (chip) gpiod_chip_close(chip);

    return 0;
}
