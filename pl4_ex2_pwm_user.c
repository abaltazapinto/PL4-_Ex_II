// pl4_ex2_pwm_user.c
// Raspberry Pi 4 - PWM0 channel 0 on GPIO12
// gcc -O2 -Wall -o pl4_ex2_pwm_user pl4_ex2_pwm_user.c

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define PERIPHERAL_BASE   0xFE000000UL
#define GPIO_OFFSET       0x00200000UL
#define PWM_OFFSET        0x0020C000UL
#define BLOCK_SIZE        4096

#define GPIO_BASE_PHYS    (PERIPHERAL_BASE + GPIO_OFFSET)
#define PWM_BASE_PHYS     (PERIPHERAL_BASE + PWM_OFFSET)

typedef struct {
    volatile uint32_t GPFSEL[6];
} bcm2711_gpio_registers_t;

typedef struct {
    volatile uint32_t CONTROL;      // CTL
    volatile uint32_t STATUS;       // STA
    volatile uint32_t DMAC;
    volatile uint32_t reserved;
    volatile uint32_t CHN0_RANGE;   // RNG1
    volatile uint32_t CHN0_DATA;    // DAT1
    volatile uint32_t FIF1;
    volatile uint32_t CHN1_RANGE;   // RNG2
    volatile uint32_t CHN1_DATA;    // DAT2
} bcm2711_pwm_registers_t;

static void gpio_set_alt0(volatile uint32_t *gpfsel, int pin)
{
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    uint32_t v = gpfsel[reg];
    v &= ~(0x7u << shift);
    v |=  (0x4u << shift);   // ALT0 = 100
    gpfsel[reg] = v;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <0..255>\n", argv[0]);
        return 1;
    }

    int duty = atoi(argv[1]);
    if (duty < 0) duty = 0;
    if (duty > 255) duty = 255;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    void *gpio_map = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fd, GPIO_BASE_PHYS);
    void *pwm_map  = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fd, PWM_BASE_PHYS);

    if (gpio_map == MAP_FAILED || pwm_map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    bcm2711_gpio_registers_t *gpio = (bcm2711_gpio_registers_t *)gpio_map;
    bcm2711_pwm_registers_t  *pwm  = (bcm2711_pwm_registers_t *)pwm_map;

    // GPIO12 -> ALT0 -> PWM0
    gpio_set_alt0(gpio->GPFSEL, 12);

    pwm->CONTROL = 0;
    usleep(10);

    pwm->STATUS = 0xFFFFFFFFu;
    usleep(10);

    pwm->CHN0_RANGE = 255;
    usleep(10);

    pwm->CHN0_DATA = (uint32_t)duty;
    usleep(10);

    pwm->CONTROL = 0x1;
    usleep(10);

    printf("PWM set on GPIO12: %d/255\n", duty);
    printf("GPFSEL1 = 0x%08X\n", gpio->GPFSEL[1]);
    printf("PWM CTL  = 0x%08X\n", pwm->CONTROL);
    printf("PWM STA  = 0x%08X\n", pwm->STATUS);
    printf("PWM RNG1 = %u\n", pwm->CHN0_RANGE);
    printf("PWM DAT1 = %u\n", pwm->CHN0_DATA);

    while (1) pause();

    return 0;
}