#include <UR_Protocol/URUS_Protocol.h>
#include "main.h"

#pragma pack(push, 1)
enum MYOBJ {
    OBJ1 = URUS_REG_COUNT,
    OBJ2
};

typedef struct __urus_test_t
{
    uint8_t reg = OBJ1;
    uint8_t cmd1 = 0;
    uint8_t cmd2 = 0;

} urus_test_t;
#pragma pack(pop)

const urus_slot_s URUS_Protocol::ur_external_slot_sizes[] = {
    add_reg_slot(OBJ1, urus_test_t)
};

int main(int argc, char* const argv[])
{
    printf("Starting...\n\n");

    URUS_Protocol ur_protocol;
    urus_gpio_t urgpio;
    urus_command_t command;
    urus_txreg_t txreg;
    urus_headerid_t urhdrid;

    urhdrid.hdrid = 0;

    ur_protocol.configure(urhdrid, UR_EXT_SLOT_SIZE);

    urus_slot_info_t urus_inf;

    printf("Creating data obj...\n\n");

    ur_protocol.Create_Obj(URUS_REG_GPIO, urus_inf);
    ur_protocol.Create_Obj(URUS_REG_COMMAND, urus_inf);
    ur_protocol.Create_Obj(URUS_REG_TXREG, urus_inf);

    switch (urus_inf.UError) {
    case slot_error_t::NONE:
        printf("None!\n");
        break;
    case slot_error_t::CREATED:
        printf("Created!\n");
        break;
    case slot_error_t::FULL:
        printf("Full!\n");
        break;
    case slot_error_t::IN_USE:
        printf("In Use!\n");
        break;
    default:
        break;
    }

    printf("\n");
    printf("Cetting data length...\n\n");

    printf("[GPIO id: 0x%02X]   [len: %u]\n", URUS_REG_GPIO, ur_protocol.Get_RegLen(URUS_REG_GPIO));
    printf("[CMD  id: 0x%02X]   [len: %u]\n", URUS_REG_COMMAND, ur_protocol.Get_RegLen(URUS_REG_COMMAND));

    urgpio.gpio = -13;
    //data for receiver
    command.cmd1 = 0x10;

    //data for sender
    txreg.TargetReg = URUS_REG_COMMAND;
    txreg.RplyHdrid = ur_protocol.Get_LocalHeaderId().hdrid;

    printf("\n---TXREG---\n");

    if (ur_protocol.Set_DataReg(txreg) == 1) {
        printf("OK txreg Setted!\n");
    } else {
        printf("NO txreg Setted!\n");
    }


    printf("\n---COMMAND---\n");

    if (ur_protocol.Set_DataReg(command) == 1) {
        printf("OK command Setted!\n");
    } else {
        printf("NO command Setted!\n");
    }

    printf("\n---GPIO---\n");

    if (ur_protocol.Set_DataReg(urgpio) == 1) {
        printf("OK gpio Setted!\n");
    } else {
        printf("NO gpio Setted!\n");
    }


    printf("\n\n");

    //ur_protocol.Get_DataReg(urgpio);
    printf("[GPIO id: 0x%02X]   [gpio_one: %d]   [gpio_two: %d]\n", urgpio.reg, urgpio.gpio, urgpio.gpio);
    printf("[CMD  id: 0x%02X]   [cmd_one: 0x%02X]\n", command.reg, command.cmd1);

    uint8_t len = ur_protocol.Get_RegLen(URUS_REG_GPIO) + ur_protocol.Get_RegLen(URUS_REG_HDRID);
    uint8_t msg[len];

    printf("\n\n");

    ur_protocol.Prepare_Msg(URUS_REG_GPIO, msg);
    printf("[id: 0x%02X]   [dat: 0x%02X]\n", msg[0], msg[1]);

    urgpio.gpio = urgpio.gpio * (-1);
    ur_protocol.Set_DataReg(urgpio);

    uint8_t len1 = ur_protocol.Get_RegLen(URUS_REG_GPIO) + ur_protocol.Get_RegLen(URUS_REG_HDRID);
    uint8_t msg1[len1];
    ur_protocol.Prepare_Msg(URUS_REG_GPIO, msg1);

    printf("\n\n");

    for (uint8_t i = 0; i < len1; i++) {
        printf("[i=%u: 0x%02X]   ", i, msg1[i]);
    }

    printf("\n");
    //txreg.TargetReg = URUS_REG_COMMAND;
    //txreg.RplyHdrid = ur_protocol.Get_LocalHeaderId().hdrid;
    //ur_protocol.Set_DataReg(txreg);
    uint8_t len2 = ur_protocol.Get_RegLen(URUS_REG_TXREG) + ur_protocol.Get_RegLen(URUS_REG_HDRID);
    uint8_t msg2[len2];
    ur_protocol.Prepare_Msg(URUS_REG_TXREG, msg2);

    for (uint8_t i = 0; i < len2; i++) {
        printf("[i=%u: 0x%02X]   ", i, msg2[i]);
    }

/*
    uint8_t lenuart = hal.uartB->available();
    if (lenuart > 0) {
        //uint8_t lenuart = (uint8_t)hal.uartB->available();
        uint8_t byte1;
        for (uint8_t i1 = 0; i1 < lenuart; i1++) {
            //byte1 = hal.uartB->read();
            ur_protocol.Input_RxData(byte1);
            //printf("i1%2u: 0x%2x ", i1, byte1);
        }
    }
*/

    printf("\n\n");

    ur_protocol.Get_DataReg(command);
    printf("[CMD  id: 0x%02X]   [cmd_one: 0x%02X]\n", command.reg, command.cmd1);

    printf("\n");
    printf("ok\n");

    return 0;
}
