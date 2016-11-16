// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
#include <stdio.h>
#ifdef APM_BUILD_DIRECTORY
#include <AP_HAL/AP_HAL.h>
const extern AP_HAL::HAL& hal;
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#include "Urus_Protocol.h"

#define DEBUG_URUS 1
#define URUS_LEN_BUFFER 16

urus_objects_t CL_UrusProtocol::_ur_objects[URUS_MAX_SLOTS];
//uint8_t *CL_UrusProtocol::rxdat;
uint8_t CL_UrusProtocol::_count_obj = 0;

CL_UrusProtocol::CL_UrusProtocol(uint16_t hdrid):
    //_count_obj(0),
    //_ur_objects(new urus_objects_t[URUS_MAX_SLOTS]),
    _rxdat(new uint8_t),
    //rxdattemp(new uint8_t[10]),
    rx_buffer(new uint8_t),
    tx_buffer(new uint8_t),
    hit_header(false),
    counter_dat(0),
    count_len(0),
    fire_data_rx(false),
    is_tx_request(false),
    fire_data_tx(false),
    _in_txbuffer(0),
    _in_rxbuffer(0)
{
    if (hdrid != 0) {
        _ur_headerid.hdrid = hdrid;
    } else {
        _ur_headerid.hdrid = URUS_HEADERID;
    }
    
    //checkendianess = CheckLittleEndianness();
}

CL_UrusProtocol::CL_UrusProtocol(void):
    //_count_obj(0),
    //_ur_objects(new urus_objects_t[URUS_MAX_SLOTS]),
    _rxdat(new uint8_t),
    //rxdattemp(new uint8_t[10]),
    rx_buffer(new uint8_t),
    tx_buffer(new uint8_t),
    hit_header(false),
    counter_dat(0),
    count_len(0),
    fire_data_rx(false),
    is_tx_request(false),
    fire_data_tx(false),
    _in_txbuffer(0),
    _in_rxbuffer(0)
{
    _ur_headerid.hdrid = URUS_HEADERID;
    //checkendianess = CheckLittleEndianness();
}

uint8_t CL_UrusProtocol::_SerializeMessage(uint8_t reg, uint8_t* msg_data, uint8_t include_hdrid)
{
    uint8_t len_urus_hdrid = sizeof(urus_headerid_t);

    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            uint8_t len_rc_chan = sizeof(urus_rc_chan_t);

            if (include_hdrid) {
                memcpy(msg_data, &_ur_headerid, len_urus_hdrid);
            }

            memcpy(&msg_data[(len_urus_hdrid * include_hdrid)], _ur_objects[0].ptr_obj, len_rc_chan);

            return 1;
      }
      default:
            return 1;
    }
    return 0;
}

uint8_t CL_UrusProtocol::Get_RegLen(uint8_t reg)
{
    uint8_t len;
    switch (reg) {

        case URUS_REG_TXREG:
        {
            len = sizeof(urus_txreg_t);
            return len;
        }

        case URUS_REG_RC_CHAN:
        {
            len = sizeof(urus_rc_chan_t);
            return len;
        }

        default:
            len = 0;
    }
    return len;
}

uint8_t CL_UrusProtocol::Set_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    //memcpy(&reg, ptr_reg, 1);
    reg = ptr_reg[0];

/*    for (uint8_t i = 0; i < _count_obj; i++) {
        if (_ur_objects[i].ptr_obj != NULL) {
            if (_ur_objects[i].reg == reg) {
                uint8_t len;
                len = _ur_objects[i].len;
                memcpy(_ur_objects[i].ptr_obj, ptr_reg, len);
                return 1;
            }
        }
    }
*/

    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(_ur_objects[0].ptr_obj, ptr_reg, sizeof(urus_rc_chan_t));
            //hal.console->printf("CH1_set: %u\n", ur->ch1);
            break;
        }
        default:
            break;
    }

    return 0;
}

uint8_t CL_UrusProtocol::Get_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    //memcpy(&reg, ptr_reg, 1);
    reg = ptr_reg[0];

/*    for (uint8_t i = 0; i < _count_obj; i++) {
        if (_ur_objects[i].ptr_obj != NULL) {
            if (_ur_objects[i].reg == reg) {
                uint8_t len;
                len = _ur_objects[i].len;
                memcpy(ptr_reg, _ur_objects[i].ptr_obj, len);
                return 1;
            }
        }
    }
*/

    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(ptr_reg, _ur_objects[0].ptr_obj, sizeof(urus_rc_chan_t));
            //hal.console->printf("CH1: %u\n", ur->ch1);
            return 1;
        }
        default:
            return 0;
    }

    return 0;
}

urus_headerid_t CL_UrusProtocol::Get_HeaderId(void)
{
    return _ur_headerid;
}

void CL_UrusProtocol::Header16_LittlEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = temp_data >> 8;
    temp_data = ((((uint16_t)rx_data) << 8) | temp_data );
}

void CL_UrusProtocol::Header16_BigEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = (uint16_t)((temp_data << 8) | rx_data );
}

void CL_UrusProtocol::RingCheck_HeaderId(uint16_t& temp_data, uint8_t& rx_data)
{
    if (checkendianess) {
        Header16_LittlEndian(temp_data, rx_data);
    } else {
        Header16_BigEndian(temp_data, rx_data);
    }

}

bool CL_UrusProtocol::CheckLittleEndianness() 
{
    uint16_t a=0x1234;
    if (*((uint8_t *)&a)==0x12) {
        checkendianess = false;
	    return false;
        //hal.console->printf("BIG\n");
	} else {
        return true;
        checkendianess = true;
        //hal.console->printf("LITTLE\n");
    }
    
}

/// <summary>
/// Prepare messages and serialize data dest.
/// Parameters.-
/// @ptr_reg   : Source data to serialize.
/// @msg_data: Destination data serialized to send.
/// </summary>
void CL_UrusProtocol::Prepare_Msg(uint8_t reg, uint8_t* msg_data)
{
    _SerializeMessage(reg, msg_data, 1);
}

uint8_t CL_UrusProtocol::Buf_Updated(BUFType typebuf)
{
    switch (typebuf) {
        case URUS_TX_BUF:
        {
            return _ur_buffer_inf.txupdated;
        }

        case URUS_RX_BUF:
        {
            return _ur_buffer_inf.rxupdated;
        }

        default:
            return 0;
    }

    return 0;
}

uint8_t CL_UrusProtocol::Get_Buf_Len(BUFType typebuf)
{

    switch (typebuf) {
        case URUS_TX_BUF:
        {
            return _ur_buffer_inf.txlen;
        }

        case URUS_RX_BUF:
        {
            return _ur_buffer_inf.rxlen;
        }

        default:
            return 0;
    }

    return 0;
}

const uint8_t* CL_UrusProtocol::Get_Buf(BUFType typebuf)
{
    _ur_buffer_inf.txupdated = 0;

    uint8_t *buffer;

    switch (typebuf) {
        case URUS_TX_BUF:
        {
            buffer = tx_buffer;
            return tx_buffer;
        }

        case URUS_RX_BUF:
        {
            buffer = rx_buffer;
            return rx_buffer;
        }

        default:
            return NULL;
    }

    return buffer;
}

uint8_t CL_UrusProtocol::Create_Obj(URUSObj obj_type, urus_slot_info_t& slot_info)
{
    slot_error_t uerror;
    slot_info.reg = obj_type;
    hal.console->printf("Slot obj_type: 0x%x\n", slot_info.reg);

    for (uint8_t i1=0; i1 < _count_obj; i1++) {
        if (_ur_objects[_count_obj].ptr_obj == NULL) {
            slot_info.UError = uerror.CREATED;
        }
    }
            switch (obj_type) {
                case URUS_REG_RC_CHAN:
                {
                    _ur_objects[_count_obj].reg = obj_type;
                    _ur_objects[_count_obj].ptr_obj = new uint8_t[sizeof(urus_rc_chan_t)];
                    _ur_objects[_count_obj].pos = _count_obj;
                    _ur_objects[_count_obj].len = sizeof(urus_rc_chan_t);
                    _count_obj++;
                    for (uint8_t i2 = 0; i2 < _count_obj; i2++) {
                        hal.console->printf("OBJECT: %x Count: %u\n", _ur_objects[i2].reg, _count_obj);
                    }
                    return 1;
                }
                default:
                    return 0;
            }
    return 0;
}

uint8_t CL_UrusProtocol::Create_Obj(uint8_t reg, urus_slot_info_t& slot_info)
{
    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            Create_Obj(URUSObj::URUS_REG_RC_CHAN, slot_info);
            hal.console->printf("Slot uint8: 0x%x\n", slot_info.reg);
            return 1;
        }
        default:
            return 0;
    }

    return 0;
}

void CL_UrusProtocol::Get_Instance(uint8_t *ptr_reg, uint8_t instance)
{
    uint8_t reg;
    reg = ptr_reg[0];
    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(ptr_reg, _ur_objects[instance].ptr_obj, sizeof(urus_rc_chan_t));
            break;
        }
        default:
            break;
    }
}


uint8_t CL_UrusProtocol::Process_Data(uint8_t data, uint8_t offset)
{
    static uint8_t counterdat = 0;
    static uint8_t count_offset = 0;
    static bool hit = false;
    static uint8_t countlen = 0;
    static uint16_t headeridcheck = 0;
    static uint16_t headerid = 0;
    static bool is_tx = false;
    static char msgbox[32];
    static bool checking_reg_len = false;

    if (_in_txbuffer ) {
        return 0;
    }
    
    if (_in_rxbuffer) {
        return 0;
    }

    if (count_offset < offset) {
        count_offset++;
        return 0;
    }

    //Check if hit with headerid
    if (!hit) {
        RingCheck_HeaderId(headerid, data);
    } //else {
        /*  We check constantly the headerid and compare if it's
            got again and check if the last data received was in process,
            if it's trigger, thats very bad and means we have a fragmented
            data, maybe master are sending very fast the enterely command
            or something else.
        */
        //RingCheck_HeaderId(headeridcheck, rx_dat);

        //if ((headerid == headeridcheck) && (hit)) {
            //hal.console->printf("Fragmented\n");
            //goto RESET_ALL;
        //}
    //}

    /* It's are jumping when a new data register are incoming,
       when hit with header id, then enter to complete register id verification,
       first we check if the register id is a command request or are sending a data
       payload, if got a command request then jump to send process the register requested,
       if we doesn't got a command request then we enter to buffering process if got it,
       then we store payload length as fast as possible in to URUS Objects slots
       if there is one space available, if not, we throw an error info.
    */

    if (headerid == (uint16_t)URUS_HEADERID) {
            if (!hit) {
                hit = true;
                checking_reg_len = true;
                return 0;
            }

            //hal.console->printf("REG: %u\n", dat1);
            if (checking_reg_len) {
                countlen = Get_RegLen(data);
                hal.console->printf("c: %u\n", countlen);
                if (countlen == 0) {
                    hal.console->printf("Reset count!\n");
                    sprintf(msgbox, "counter");
                    goto RESET_ALL2;
                }
            }

            if ((data == _ur_txreg.reg) && (is_tx == false) && (countlen <= sizeof(_ur_txreg))) {
                is_tx = true;
                //is_tx_request = is_tx;
                return 0;
            }

            if (is_tx) {
                checking_reg_len = false;
                delete tx_buffer;
                tx_buffer = new uint8_t[countlen];
                _ur_buffer_inf.txlen = countlen;
                tx_buffer[0] = data;
                _ur_txreg.TargetReg = data;
                hal.console->printf_P(PSTR("ProcTX Reg:0x%x Targe: 0x%x\n"), _ur_txreg.reg, _ur_txreg.TargetReg);
                //is_tx = false;
                _in_txbuffer = 1;
                sprintf(msgbox, "Is_tx" );
                goto RESET_ALL2;
            }

            checking_reg_len = false;
            delete rx_buffer;
            rx_buffer = new uint8_t[countlen];
            _ur_buffer_inf.rxlen = countlen;
            rx_buffer[0] = data;
            _in_rxbuffer = 1;

        if (hit) {
            //hit = false;
            //hal.console->printf("RECIBIDO SlotError: %u \n", slotinfo.UError);
            //hal.console->printf("RECIBIDO dat0:%x dat1:%x dat2:%x dat3:%x dat4:%x dat5:%x dat6:%x SPSR:%x \n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4], rx_buffer[5], rx_buffer[6], headerid); 
            sprintf(msgbox, "recibido hit" );
            
            goto RESET_ALL2;

        }
    }

return 0;

RESET_ALL2:
    counterdat = 0;
    counter_dat = counterdat;
    hit = false;
    hit_header = hit;
    headerid = 0;
    headeridcheck = 0;
    countlen = 0;
    count_len = countlen;
    fire_data_rx = false;
    count_offset = 0;
    is_tx = false;
    checking_reg_len = false;
    hal.console->printf("all reset %s! \n", msgbox);
return 1;
}

uint8_t CL_UrusProtocol::Input_RxData(uint8_t data)
{

    Process_Data(data, 0);
    Poll_Rx_Buffer2(data);

    return 0;
}

uint8_t CL_UrusProtocol::Poll_Rx_Buffer2(uint8_t data)
{

    if (_in_rxbuffer) {
            counter_dat++;
            
        if (counter_dat < (Get_RegLen(rx_buffer[0]))) {
            rx_buffer[counter_dat] = data;

        }

        if (counter_dat >= Get_RegLen(rx_buffer[0])) {
            _in_rxbuffer = 0;
            urus_slot_info_t slotinfo;
            Create_Obj(rx_buffer[0], slotinfo);
            _ur_buffer_inf.rxupdated = Set_DataReg(&rx_buffer[0]);
            hal.console->printf("RECIBIDO dat0:%x dat1:%x dat2:%x dat3:%x dat4:%x dat5:%x dat6:%x \n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4], rx_buffer[5], rx_buffer[6]); 
            for (uint8_t i = 0; i < Get_RegLen(rx_buffer[0]); i++) {
                rx_buffer[i] = 0;
            }
        }
        
    }

    return 0;
}

uint8_t CL_UrusProtocol::Poll_Tx_Buffer2()
{

    if (_in_txbuffer) {

        _ur_buffer_inf.txupdated = _SerializeMessage(_ur_txreg.TargetReg, tx_buffer, 0);
        hal.console->printf("Listo TX dat0:%x dat1:%x dat2:%x dat3:%x dat4:%x dat5:%x dat6:%x \n", tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3], tx_buffer[4], tx_buffer[5], tx_buffer[6]); 
        _in_txbuffer = 0;
    }

    return 0;
}
