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
    volatile uint32_t CONTROL;
    volatile uint32_t STATUS;
    volatile uint32_t DMAC[2];
    volatile uint32_t CHN0_RANGE;
    volatile uint32_t CHN0_DATA;
    volatile uint32_t FIF1[2];
    volatile uint32_t CHN1_RANGE;
    volatile uint32_t CHN1_DATA;
} bcm2711_pwm_registers_t;

static void gpio_set_alt0(volatile uint32_t *gpfsel, int pin) {
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    uint32_t v = gpfsel[reg];
    v &= ~(0x7u << shift);
    v |=  (0x4u << shift);   // ALT0 = 100
    gpfsel[reg] = v;
}

int main(int argc, char **argv) {

int duty = 128; // default

if (argc == 2) {
    duty = atoi(argv[1]);
    if (duty < 0) duty = 0;
    if (duty > 255) duty = 255;
}

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    void *gpio_map = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_BASE_PHYS);
    void *pwm_map  = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PWM_BASE_PHYS);

    if (gpio_map == MAP_FAILED || pwm_map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    bcm2711_gpio_registers_t *gpio = (bcm2711_gpio_registers_t *)gpio_map;
    bcm2711_pwm_registers_t  *pwm  = (bcm2711_pwm_registers_t  *)pwm_map;

    // GPIO12 -> ALT0 -> PWM0 CH0
    gpio_set_alt0(gpio->GPFSEL, 12);

    // 256 níveis
    pwm->CHN0_RANGE = 255;

    // ativar PWM0 canal 0 (bit PWEN1)
    pwm->CONTROL |= 0x1;

    // duty cycle
    pwm->CHN0_DATA = (uint32_t)duty;

    printf("PWM set on GPIO12: %d/255\n", duty);

    while (1) {
    int new_duty;

    printf("Duty cycle (0..255, -1 para sair): ");
    
    fflush(stdout);

        if (scanf("%d", &new_duty) != 1)
            break;

        if (new_duty == -1)
            break;

        if (new_duty < 0) new_duty = 0;
        if (new_duty > 255) new_duty = 255;

        pwm->CHN0_DATA = (uint32_t)new_duty;

        printf("PWM set on GPIO12: %d/255\n", new_duty);
    }

    return 0;
}