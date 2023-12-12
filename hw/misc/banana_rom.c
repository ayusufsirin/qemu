#include "qemu/osdep.h"
#include "qapi/error.h" /* provides error_fatal() handler */
#include "hw/sysbus.h" /* provides all sysbus registering func */
#include "hw/misc/banana_rom.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> // For memset

static int createSocket(void) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Initialize serv_addr

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        close(sock);
        return -1;
    }
    return sock;
}

#define TYPE_BANANA_ROM "banana_rom"
typedef struct BananaRomState BananaRomState;
DECLARE_INSTANCE_CHECKER(BananaRomState, BANANA_ROM, TYPE_BANANA_ROM)

#define REG_ID 	0x0
#define CHIP_ID	0xBA000001

struct BananaRomState {
	SysBusDevice parent_obj;
	MemoryRegion iomem;
	uint64_t chip_id;
};

static uint64_t banana_rom_read(void *opaque, hwaddr addr, unsigned int size) {
    int sock = createSocket();
    if (sock < 0) {
        return 0;
    }

    const char* command = "read_reg";
    send(sock, command, strlen(command), 0);

    int value = 0;
    ssize_t ret = read(sock, &value, sizeof(value));
    if (ret < 0) {
        // Handle read error
        perror("Read failed");
    }

    close(sock);
    return value;
}

static const MemoryRegionOps banana_rom_ops = {
	.read = banana_rom_read,
	.endianness = DEVICE_NATIVE_ENDIAN,
};

static void banana_rom_instance_init(Object *obj)
{
	BananaRomState *s = BANANA_ROM(obj);

	/* allocate memory map region */
	memory_region_init_io(&s->iomem, obj, &banana_rom_ops, s, TYPE_BANANA_ROM, 0x100);
	sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->iomem);

	s->chip_id = CHIP_ID;
}

/* create a new type to define the info related to our device */
static const TypeInfo banana_rom_info = {
	.name = TYPE_BANANA_ROM,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(BananaRomState),
	.instance_init = banana_rom_instance_init,
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
