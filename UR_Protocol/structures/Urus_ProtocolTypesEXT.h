/*
 * This file is part of the Urus Protocol and licensed under the GNU Lesser General Public License, version 2.1
 * https://www.gnu.org/licenses/lgpl-2.1.html
 */

#pragma once

/**************************************************************************
 *
 *                          URUS_REG_CORE_EXTERNAL
 *
 **************************************************************************/

/// <summary>
/// Here we declare all GENERAL registers objects that we need.
/// You can add N registers objects, please be becarefull on adding.
/// The only restriction is the RAM limit of the target board
/// and you can slot in up to 255.
/// </summary>

/// Register object for RC_CHANNELS
/// OBJECT NAME: "urus_rc_chan_t".
/// @reg Address register.
/// @CH1 Field to
/// @CH2 are RC_CHANNEL.
typedef struct __urus_rc_chan_t
{
    uint8_t reg;
    uint16_t CH1;
	uint16_t CH2;
    uint16_t CH3;
	uint16_t CH4;
	uint16_t CH5;
	uint16_t CH6;

public:
	__urus_rc_chan_t():
        reg(URUS_REG_RC_CHAN),
        CH1(0),
		CH2(0),
        CH3(0),
		CH4(0),
		CH5(0),
		CH6(0)
	{}

} urus_rc_chan_t;

typedef struct __urus_gpio_t
{
    uint8_t reg;
    int8_t gpio;

public:
	__urus_gpio_t():
        reg(URUS_REG_GPIO),
        gpio(0)
	{}

} urus_gpio_t;

typedef struct __urus_command_t
{
    uint8_t reg;
    uint8_t cmd1;
    uint8_t cmd2;
    uint8_t cmd3;
    uint8_t cmd4;

public:
	__urus_command_t():
        reg(URUS_REG_COMMAND),
        cmd1(0),
        cmd2(0),
        cmd3(0),
        cmd4(0)
	{}

} urus_command_t;

/**************************************************************************
 *
 *                          URUS_REG_CORE_EXTERNAL
 *
 **************************************************************************/
