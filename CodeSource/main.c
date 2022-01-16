#include <string.h>

#include "board.h"
#include "xtimer.h"
#include "ztimer.h"

/* Add necessary include here */
#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
#include "lm75.h"
#include "lm75_params.h"

#include "periph/gpio.h"
#include "periph_conf.h"

#include "cayenne_lpp.h"
//#include "periph/rtc.h"
#include "thread.h"


/* Declare globally the sensor device descriptor */
//extern semtech_loramac_t loramac;  /* The loramac stack descriptor */
static lm75_t lm75;

/* Device and application informations required for OTAA activation */
/*static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0xe7, 0x10, 0xe3, 0xf7, 0xd7, 0x79, 0x00, 0xc6 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x23, 0xea, 0x5e, 0x66, 0x40, 0x5e, 0xfa, 0x47, 0x56, 0xc0, 0xfb, 0x00, 0x82, 0x98, 0xf3, 0x1f };

static cayenne_lpp_t lpp;*/

static volatile int flag;
static volatile int flag_buzzer;

static volatile kernel_pid_t pid;

static char stack[THREAD_STACKSIZE_MAIN];

gpio_t pin = GPIO_PIN(1,5);
gpio_t pin_button = GPIO_PIN(0,9);
gpio_t pin_buzzer = GPIO_PIN(0,0);

void release_button(void *arg){
	(void) arg;
	gpio_irq_enable(pin_button);	
}

static void generate_osc(int freq, int duree, float alpha){
	int d_init_tot=(int)xtimer_now_usec();
	int d=0;
	int periode=1000000/freq;
	int demi_periode=(int)((float)periode*alpha);
	while (flag==0 && d<duree*1000){
		int periode_init=(int)xtimer_now_usec();
		int d_periode=0;
		while (flag==0 && d_periode<demi_periode) {
			flag_buzzer=1;
			gpio_write(pin_buzzer,flag_buzzer);
			d_periode=(int)xtimer_now_usec()-periode_init;
		}
		while (flag==0 && d_periode<periode) {
			flag_buzzer=0;
			gpio_write(pin_buzzer,flag_buzzer);
			d_periode=(int)xtimer_now_usec()-periode_init;
		}
		d=(int)xtimer_now_usec()-d_init_tot;
	}
}

static void *thread_handler(void *arg)
{
    (void)arg;
	while (1) {
		while (flag==0) {
			generate_osc(554,100,0.5);	
			generate_osc(440,400,0.5);	
		}
		gpio_write(pin_buzzer,0);
		thread_sleep();
	}
    return NULL;
}

static ztimer_t react_button = { .callback = release_button};

static void button_handler(void *arg) {
	printf("%d\n",(int)arg);
	if (flag == 0) flag=1;
	else flag=0;
	gpio_write(pin,flag);
	if (flag==0) thread_wakeup(pid);
	gpio_irq_disable(pin_button);
	ztimer_set(ZTIMER_MSEC,&react_button,500);
}


int main(void)
{
	flag=1;
	int val_led = 0;
	
    /* configure the device parameters */
    if (lm75_init(&lm75, &lm75_params[0]) != LM75_SUCCESS) {
        puts("Sensor initialization failed");
        return 1;
    }
	
	//struct tm time;
    //rtc_get_time(&time);
	
	
	gpio_init(pin,GPIO_OUT);
	gpio_init_int(pin_button,GPIO_IN,GPIO_FALLING,button_handler, (void *)val_led);
	gpio_init(pin_buzzer,GPIO_OUT);

    /*semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);*/
    
    /* change datarate to DR5 (SF7/BW125kHz) */
    //semtech_loramac_set_dr(&loramac, 5);
    
    /* start the OTAA join procedure *//*
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");*/
	
	pid=thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN-1, 0, thread_handler, NULL, "new thread");
	
	
    
    while (1) {
        /* do some measurements */
        int temperature = 0;
        if (lm75_get_temperature(&lm75, &temperature)!= LM75_SUCCESS) {
            puts("Cannot read temperature!");
        }
		printf("Temp : %d.%d C\n",temperature/1000,temperature%1000);
		if (temperature>25000){
			flag=0;
			gpio_write(pin,flag);
			thread_wakeup(pid);
		}
		
		/*cayenne_lpp_add_temperature(&lpp, 0, (float)temperature / 1000);

        char message[32];
        sprintf(message, "T:%d.%dC",
                (temperature / 1000), (temperature % 1000));
        printf("Sending message '%s'\n", message);
*/
        /* send the message here *//*
        uint8_t ret = semtech_loramac_send(&loramac, lpp.buffer, lpp.cursor);
        if (ret != SEMTECH_LORAMAC_TX_DONE) {
            printf("Cannot send lpp message, ret code: %d\n", ret);
        }
		
		printf("Sending LPP data\n");
		*/
		
		
		//generate_osc(554,100,0.5);
		//val_led=1-val_led;
        
		//cayenne_lpp_reset(&lpp);
		
        /* wait 20 seconds between each message */
        xtimer_sleep(5);
    }

    return 0; /* should never be reached */
}
