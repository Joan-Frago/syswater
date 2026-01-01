#ifndef UNIPI_CONTROL_H
#define UNIPI_CONTROL_H

#define XML_XPATH_EXPR_SIZE 255

typedef struct Relay{
	char *id_pin; // e.g. RO2.1
	char *pin; // e.g. 2.1
} rl_t;

typedef struct DigitalInput{
	char *id_pin;
	char *pin;
} di_t;

typedef struct Historify{
	int active;
	int period;
} historify_t;

typedef struct Date{
	char *start;
	char *end;
} date_t;

typedef struct Fire_Device{
	int active;
	int period;
	date_t date;

} fire_device_t;

int relay_write(struct Relay *, int);
int digital_read(struct DigitalInput *, int *);

void *core(void*);

#endif
