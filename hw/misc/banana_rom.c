#include "qemu/osdep.h"
#include "qapi/error.h" /* provides error_fatal() handler */
#include "hw/sysbus.h" /* provides all sysbus registering func */
#include "hw/misc/banana_rom.h"

#include "hw/fpga/simple_module/obj_dir/Vsimple_module.h"

#define TYPE_BANANA_ROM "banana_rom"
typedef struct BananaRomState BananaRomState;
DECLARE_INSTANCE_CHECKER(BananaRomState, BANANA_ROM, TYPE_BANANA_ROM)

#define REG_ID 	0x0
#define REG_COUNTER 0x08
#define CHIP_ID	0xBA000001

struct BananaRomState {
	SysBusDevice parent_obj;
	MemoryRegion iomem;
	uint64_t chip_id;

	Vsimple_module *verilated_module;
	bool enable;
};

static uint64_t banana_rom_read(void *opaque, hwaddr addr, unsigned int size)
{
	BananaRomState *s = opaque;

	s->verilated_module->clk = 1;
    s->verilated_module->eval();
    s->verilated_module->clk = 0;
    s->verilated_module->eval();

	switch (addr) {
	case REG_ID:
		return s->chip_id;
		break;

	case REG_COUNTER:
        return s->verilated_module->counter;
        break;

	default:
		return 0xDEADBEEF;
		break;
	}

	return 0;
}

static void banana_rom_write(void *opaque, hwaddr addr, uint64_t value, unsigned size)
{
    BananaRomState *s = opaque;

    switch (addr) {
    case REG_ENABLE:
        s->enable = value;
        s->verilated_module->enable = s->enable;
        break;

    default:
        // Handle other writes or ignore
        break;
    }
}

static void banana_rom_reset(DeviceState *dev)
{
    BananaRomState *s = BANANA_ROM(dev);

    s->verilated_module->reset = 1;
    s->verilated_module->eval();
    s->verilated_module->reset = 0;
    s->verilated_module->eval();
}

static const MemoryRegionOps banana_rom_ops = {
	.read = banana_rom_read,
	.write = banana_rom_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
};

static void banana_rom_instance_init(Object *obj)
{
	BananaRomState *s = BANANA_ROM(obj);

	/* allocate memory map region */
	memory_region_init_io(&s->iomem, obj, &banana_rom_ops, s, TYPE_BANANA_ROM, 0x100);
	sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

	s->chip_id = CHIP_ID;
	s->verilated_module = new Vsimple_module();
    s->enable = false;
}

/* create a new type to define the info related to our device */
static const TypeInfo banana_rom_info = {
	.name = TYPE_BANANA_ROM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(BananaRomState),
	.instance_init = banana_rom_instance_init,
	.reset = banana_rom_reset,
};

static void banana_rom_register_types(void)
{
    type_register_static(&banana_rom_info);
}

type_init(banana_rom_register_types)

/*
 * Create the Banana Rom device.
 */
DeviceState *banana_rom_create(hwaddr addr)
{
	DeviceState *dev = qdev_new(TYPE_BANANA_ROM);
	sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
	sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr);
	return dev;
}
