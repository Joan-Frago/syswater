#ifndef DEVICE_H

#include "./unipi_control.h"

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xpath.h>

#define DEVICE_H

#define MAX_DEVICES 24
#define DEVICE_NAME_SIZE 64
#define DEVICE_DESC_SIZE 1024

typedef struct Device{
	int id;
	char *name;
	char *description;
	historify_t hist;
	fire_device_t fire;

	rl_t rl;
	di_t di;
} device_t;

int set_devices(struct Device devices[MAX_DEVICES]);

int get_all_devices(char *resp_buf);
int set_device(xmlNode *);

#endif
