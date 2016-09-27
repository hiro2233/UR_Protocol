/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#pragma once

#ifdef APM_BUILD_DIRECTORY
#include <AP_HAL/AP_HAL.h>
const extern AP_HAL::HAL& hal;
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#include "Urus_Protocol_Types.h"

class CL_UrusProtocol {
  public:
    CL_UrusProtocol(void);
    CL_UrusProtocol(uint16_t hdrid);

    uint8_t Get_RegLen(uint8_t reg);
    urus_headerid_t Get_HeaderId(void);

    template<typename A>
    uint8_t Get_DataReg(A &ptr_reg) { return _Get_DataReg((uint8_t*)&ptr_reg); };

    template<typename A>
    uint8_t Set_DataReg(A &ptr_reg) { return _Set_DataReg((uint8_t*)&ptr_reg); };

    template<typename T>
    void caca(T &func) {
        uint8_t* buf;
        uint8_t len;
        func(buf, len); 
    };

    void Get_Instance(uint8_t* ptr_reg, uint8_t instance);
    uint8_t Create_Obj(URUSObj obj_type, urus_slot_info_t& slot_info);
    uint8_t Create_Obj(uint8_t reg, urus_slot_info_t& slot_info);

    //template<typename A>
    //static uint8_t Create_Object(A &ref_obj);

    void Header16_LittlEndian(uint16_t& temp_data, uint8_t& rx_dat);
    void Header16_BigEndian(uint16_t& temp_data, uint8_t& rx_dat);
    void RingCheck_HeaderId(uint16_t& temp_data, uint8_t& rx_dat);

    poll_rx_tx_info_t Poll_Tx_CommandRequest();
    uint8_t Poll_Rx_Buffer();
    uint8_t Poll_Rx_Buffer2(uint8_t data);
    uint8_t Poll_Tx_Buffer2();

    poll_rx_tx_info_t Poll_RxTxData(uint8_t data);

    void Prepare_Msg(uint8_t reg, uint8_t* msg_data);
    uint8_t Process_RxTxData(uint8_t rx_dat, uint8_t offset_rx);

    uint8_t Input_RxData(uint8_t data);
    uint8_t Process_Data(uint8_t data, uint8_t offset);

    uint8_t Buf_Updated(BUFType typebuf);
    uint8_t Get_Buf_Len(BUFType typebuf);
    const uint8_t* Get_Buf(BUFType typebuf);

    bool CheckLittleEndianness(void);
    uint8_t* get_data1p(uint8_t instance){ return _ur_objects[instance].ptr_obj; };
    bool checkendianess;
    //uint8_t *rxdattemp;

    uint8_t *rx_buffer;
    uint8_t *tx_buffer;

    bool hit_header;
    uint8_t counter_dat;
    uint8_t count_len;
    bool fire_data_rx;
    bool is_tx_request;
    bool fire_data_tx;

  private:

    uint8_t _SerializeMessage(uint8_t ptr_reg, uint8_t* msg_data, uint8_t include_hdrid);
    static uint8_t _Set_DataReg(uint8_t* ptr_reg);
    static uint8_t _Get_DataReg(uint8_t* ptr_reg);

    urus_headerid_t _ur_headerid;
    static urus_objects_t _ur_objects[URUS_MAX_SLOTS];
    static uint8_t _count_obj;
    urus_txreg_t _ur_txreg;
    uint8_t *_rxdat;
    urus_buffer_info_t _ur_buffer_inf;
    uint8_t _in_txbuffer;
    uint8_t _in_rxbuffer;

};
