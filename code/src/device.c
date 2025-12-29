#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xpathInternals.h>
#include "../inc/unipi_control.h"
#include "../inc/util.h"

int read_devices_xml(struct Device devices[MAX_DEVICES]);
int read_device_id(struct Device devices[MAX_DEVICES], xmlNode *, int);
int read_device_name(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_description(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_historify(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_fire(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_fire_date(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_relay(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);
int read_device_digital_input(struct Device devices[MAX_DEVICES], xmlXPathContext *, int);

/*
 * Set all devices before running.
 * Read from devices xml and init Devices.
 */
int set_devices(struct Device devices[MAX_DEVICES]){
	printf("Setting devices...\n");

	if(read_devices_xml(devices) == -1){
		printf("Error: Could not read devices xml.\n");
		return -1;
	}

	printf("All devices have been set.\n\n");
	return 0;
}

int read_devices_xml(struct Device devices[MAX_DEVICES]){
	xmlDoc *devices_doc = xmlReadFile(XML_DEVICES_PATH, NULL, 0);
	if(devices_doc == NULL){
		printf("Could not parse devices configuration xml file at \"%s\"\n",XML_DEVICES_PATH);
		return -1;
	}

	xmlXPathContext *xpath_ctx = xmlXPathNewContext(devices_doc);
	if(xpath_ctx == NULL){
		printf("Error: Unable to create new XPath context.\n");
		return -1;
	}

	xmlNode *root = xmlDocGetRootElement(devices_doc);

	int dev_idx = 0;
	for(xmlNode *device = root->children; device != NULL; device = device->next){
		if(device->type == XML_ELEMENT_NODE){
			if(dev_idx >= MAX_DEVICES) break;

			xpath_ctx->node = device;
			read_device_id(devices, device, dev_idx);
			read_device_name(devices, xpath_ctx, dev_idx);
			read_device_description(devices, xpath_ctx, dev_idx);
			read_device_historify(devices, xpath_ctx, dev_idx);
			read_device_fire(devices, xpath_ctx, dev_idx);
			read_device_relay(devices, xpath_ctx, dev_idx);
			read_device_digital_input(devices, xpath_ctx, dev_idx);

			dev_idx++;
		}
	}

	xmlFreeDoc(devices_doc);
	xmlXPathFreeContext(xpath_ctx);

	printf("Finished reading xml\n");
	
	return 0;
}

int read_device_id(struct Device devices[MAX_DEVICES], xmlNode *dev_node, int dev_idx){
	xmlChar *id = xmlGetProp(dev_node, BAD_CAST "id");
	devices[dev_idx].id = char2int((char *)id);
	xmlFree(id);

	printf("Device [%d] id: %d\n", dev_idx, devices[dev_idx].id);

	return 0;
}

int read_device_name(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	char *expr = "./name";
	xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST expr, xpath_ctx);

	xmlNode *node = xpath_obj->nodesetval->nodeTab[0];
	xmlChar *content = xmlNodeGetContent(node);

	devices[dev_idx].name = strdup((char *)content);

	printf("Device [%d] name: %s\n", dev_idx, devices[dev_idx].name);

	xmlFree(content);
	xmlXPathFreeObject(xpath_obj);

	return 0;
}

int read_device_description(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	char *expr = "./description";
	xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST expr, xpath_ctx);

	xmlNode *node = xpath_obj->nodesetval->nodeTab[0];
	xmlChar *content = xmlNodeGetContent(node);

	devices[dev_idx].description = strdup((char *)content);

	printf("Device [%d] description: %s\n", dev_idx, devices[dev_idx].description);

	xmlFree(content);
	xmlXPathFreeObject(xpath_obj);

	return 0;
}

int read_device_historify(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	char *expr = "./historify";
	xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST expr, xpath_ctx);

	xmlNode *node = xpath_obj->nodesetval->nodeTab[0];
	xmlChar *active = xmlGetProp(node, BAD_CAST "active");
	xmlChar *period = xmlGetProp(node, BAD_CAST "period"); // Period must be between 0 and 9

	devices[dev_idx].hist.active = char2int((char *)active);
	devices[dev_idx].hist.period = char2int((char *)period);

	printf("Device [%d] historify: active=%d period=%d\n", dev_idx, devices[dev_idx].hist.active, devices[dev_idx].hist.period);

	xmlFree(active);
	xmlFree(period);
	xmlXPathFreeObject(xpath_obj);

	return 0;
}

int read_device_fire(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	char *expr = "./fire";
	xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST expr, xpath_ctx);

	xmlNode *node = xpath_obj->nodesetval->nodeTab[0];
	xmlChar *active = xmlGetProp(node, BAD_CAST "active");
	xmlChar *period = xmlGetProp(node, BAD_CAST "period");

	devices[dev_idx].fire.active = char2int((char *)active);
	devices[dev_idx].fire.period = char2int((char *)period);

	printf("Device [%d] fire: active=%d period=%d\n", dev_idx, devices[dev_idx].fire.active, devices[dev_idx].fire.period);

	read_device_fire_date(devices, xpath_ctx, dev_idx);

	xmlFree(active);
	xmlFree(period);
	xmlXPathFreeObject(xpath_obj);

	return 0;
}

int read_device_fire_date(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	xmlXPathObjectPtr xpath_obj_start = xmlXPathEvalExpression(BAD_CAST "./fire/date/start", xpath_ctx);
	xmlNode *date_start = xpath_obj_start->nodesetval->nodeTab[0];

	xmlXPathObjectPtr xpath_obj_end = xmlXPathEvalExpression(BAD_CAST "./fire/date/end", xpath_ctx);
	xmlNode *date_end = xpath_obj_end->nodesetval->nodeTab[0];

	devices[dev_idx].fire.date.start = (char *)xmlNodeGetContent(date_start);
	devices[dev_idx].fire.date.end = (char *)xmlNodeGetContent(date_end);

	printf("Device [%d] fire date: start=%s end=%s\n", dev_idx, devices[dev_idx].fire.date.start, devices[dev_idx].fire.date.end);

	xmlXPathFreeObject(xpath_obj_start);
	xmlXPathFreeObject(xpath_obj_end);

	return 0;
}

int read_device_relay(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	xmlXPathObjectPtr xpath_obj_relay = xmlXPathEvalExpression(BAD_CAST "./relay", xpath_ctx);
	if(xpath_obj_relay && !xmlXPathNodeSetIsEmpty(xpath_obj_relay->nodesetval)){
		xmlNode *node_relay = xpath_obj_relay->nodesetval->nodeTab[0];

		devices[dev_idx].rl.id_pin = (char *)xmlGetProp(node_relay, BAD_CAST "id_pin");
		devices[dev_idx].rl.pin = (char *)xmlGetProp(node_relay, BAD_CAST "pin");

		printf("Device [%d] relay: id_pin=%s pin=%s\n", dev_idx, devices[dev_idx].rl.id_pin, devices[dev_idx].rl.pin);
	}
	if(xpath_obj_relay) xmlXPathFreeObject(xpath_obj_relay);

	return 0;
}

int read_device_digital_input(struct Device devices[MAX_DEVICES], xmlXPathContext *xpath_ctx, int dev_idx){
	xmlXPathObjectPtr xpath_obj_relay = xmlXPathEvalExpression(BAD_CAST "./digital_input", xpath_ctx);
	if(xpath_obj_relay && !xmlXPathNodeSetIsEmpty(xpath_obj_relay->nodesetval)){
		xmlNode *node_relay = xpath_obj_relay->nodesetval->nodeTab[0];

		devices[dev_idx].di.id_pin = (char *)xmlGetProp(node_relay, BAD_CAST "id_pin");
		devices[dev_idx].di.pin = (char *)xmlGetProp(node_relay, BAD_CAST "pin");

		printf("Device [%d] digital_input: id_pin=%s pin=%s\n", dev_idx, devices[dev_idx].di.id_pin, devices[dev_idx].di.pin);
	}
	if(xpath_obj_relay) xmlXPathFreeObject(xpath_obj_relay);

	return 0;
}
