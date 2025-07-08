
/**
 * PD_UFP.h
 *
 *      Author: Ryan Ma
 *      Edited: Kai Liebich
 *
 * Minimalist USB PD Ardunio Library for PD Micro board
 * Only support UFP(device) sink only functionality
 * Requires FUSB302_UFP.h, PD_UFP_Protocol.h and Standard Arduino Library
 *
 * Support PD3.0 PPS
 * 
 */

#ifndef PD_UFP_H
#define PD_UFP_H

#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>

#include "FUSB302_UFP.h" // This header should define FUSB302_t
#include "PD_UFP_Protocol.h"

enum {
    STATUS_POWER_NA = 0,
    STATUS_POWER_TYP,
    STATUS_POWER_PPS
};
typedef uint8_t status_power_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
// PD_UFP_c
///////////////////////////////////////////////////////////////////////////////////////////////////
class PD_UFP_c
{
    public:
        PD_UFP_c();
        // Init
        void init(uint8_t int_pin, enum PD_power_option_t power_option = PD_POWER_OPTION_MAX_5V);
        void init_PPS(uint8_t int_pin, uint16_t PPS_voltage, uint8_t PPS_current, enum PD_power_option_t power_option = PD_POWER_OPTION_MAX_5V);
        // Task
        void run(void);// PD Policy task, handles power related timeouts and FUSB302 events
        // Status
        bool is_power_ready(void) { return status_power == STATUS_POWER_TYP; }
        bool is_PPS_ready(void)   { return status_power == STATUS_POWER_PPS; }
        bool is_ps_transition(void) { return send_request || wait_ps_rdy; }
        // Get
        uint16_t get_voltage(void) { return ready_voltage; }    // Voltage in 50mV units, 20mV(PPS)
        uint16_t get_current(void) { return ready_current; }    // Current in 10mA units, 50mA(PPS)
        status_power_t get_ps_status(void) { return status_power; }

        uint8_t get_src_cap_count(void) { return protocol.power_data_obj_count; }//New Function:Get source capability count
        uint8_t get_selected_position(void) { int Position = 1+ (PD_protocol_get_selected_power(&protocol)); return Position;}//Nw Function:Get selected power position 0保留所以加1
        uint8_t get_cc_pin(); //New Function:Get CC pin status PD_UFP_c 115Lines
        bool set_PPS(uint16_t PPS_voltage, uint8_t PPS_current);
        void set_power_option(enum PD_power_option_t power_option);
        // Clock
        static void clock_prescale_set(uint8_t prescaler);

        //New Function E-Marker Info Function
        void set_emarker_info(uint16_t vid, uint16_t pid, uint8_t xid = 0) {
        protocol.emarker_info.vid = vid;
        protocol.emarker_info.pid = pid;
        protocol.emarker_info.xid = xid;
        }

        //Listen Function
        void set_listen_mode(bool enable) { listen_mode = enable; }
        void Listen_init_PPS(uint8_t int_pin, uint16_t PPS_voltage, uint8_t PPS_current, enum PD_power_option_t power_option = PD_POWER_OPTION_MAX_5V);//PD_UFP.cpp 155 Lines
        void Listen_run(void);
    protected:
        static FUSB302_ret_t FUSB302_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t count);
        static FUSB302_ret_t FUSB302_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t count);
        static FUSB302_ret_t FUSB302_delay_ms(uint32_t t);
        void handle_protocol_event(PD_protocol_event_t events);
        // void ListenHandle_protocol_event(PD_protocol_event_t events);// Only protocol parsing, no power related timeout handling
        void handle_FUSB302_event(FUSB302_event_t events);
        void listen_handle_FUSB302_event(FUSB302_event_t events); //Cpp 318 Lines
        bool timer(void);// PD Policy timer, handles power related timeouts
        bool ListenTimer(void); // Only protocol parsing, no power related timeout handling
        void set_default_power(void);
        // Device
        FUSB302_dev_t FUSB302;
        PD_protocol_t protocol;
        uint8_t int_pin;
        // Power ready power
        uint16_t ready_voltage;
        uint16_t ready_current;
        // PPS setup
        uint16_t PPS_voltage_next;
        uint8_t PPS_current_next;
        // Status
        virtual void status_power_ready(status_power_t status, uint16_t voltage, uint16_t current);
        uint8_t status_initialized;
        uint8_t status_src_cap_received;
        status_power_t status_power;
        // Timer and counter for PD Policy
        uint16_t time_polling;
        uint16_t time_wait_src_cap;
        uint16_t time_wait_ps_rdy;
        uint16_t time_PPS_request;
        uint8_t get_src_cap_retry_count;
        uint8_t wait_src_cap;
        uint8_t wait_ps_rdy;
        uint8_t send_request;
        static uint8_t clock_prescaler;
        // Time functions        
        void delay_ms(uint16_t ms);
        uint16_t clock_ms(void);
        // Status logging
        virtual void status_log_event(uint8_t status, uint32_t * obj = 0) {}
        //Listen variable
        bool listen_mode = false;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Optional: PD_UFP_Log_c, extended from PD_UFP_c to provide logging function.
//           Asynchronous, minimal impact on PD timing.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct status_log_t {
    uint16_t time;
    uint16_t msg_header;
    uint8_t obj_count;
    uint8_t status;
};

// typedef struct {
//     uint8_t status;
//     uint16_t msg_header;
//     uint8_t obj_count;
//     uint32_t time;
//     const char* custom_message; // Add custom message pointer
// } Listen_status_log_t;

enum pd_log_level_t {
    PD_LOG_LEVEL_INFO,
    PD_LOG_LEVEL_VERBOSE
};

class PD_UFP_Log_c : public PD_UFP_c
{
    public:
        PD_UFP_Log_c(pd_log_level_t log_level = PD_LOG_LEVEL_INFO);
        // Task
        //void print_status(Serial_ & serial);
        void print_status(HardwareSerial & serial);
        // Get
        int status_log_readline(char * buffer, int maxlen);
        // int Listen_status_log_readline(char * buffer, int maxlen);

    protected:
        int status_log_readline_msg(char * buffer, int maxlen, status_log_t * log);
        int status_log_readline_src_cap(char * buffer, int maxlen);
        // Status log functions
        uint8_t status_log_obj_add(uint16_t header, uint32_t * obj);
        virtual void status_log_event(uint8_t status, uint32_t * obj);
        // virtual void Listen_status_log_event(uint8_t status, const char* message = nullptr, uint32_t *obj = 0) {}//监听
        // status log event queue
        status_log_t status_log[16];    // array size must be power of 2 and <=256
        uint8_t status_log_read;
        uint8_t status_log_write;
        // status log object queue
        uint32_t status_log_obj[16];    // array size must be power of 2 and <=256
        uint8_t status_log_obj_read;
        uint8_t status_log_obj_write;
        // state variables
        pd_log_level_t status_log_level;
        uint8_t status_log_counter;        
        char status_log_time[8];
};

#endif

