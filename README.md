# PL4 - Exercício II: PWM em User Space (Raspberry Pi 4)

```c
/*
 * pl4_ex2_pwm_user.c
 * Programa referente à ficha PL4 – Exercício II
 * Implementação de controlo de PWM em user space através de memory-mapped I/O (/dev/mem)
 * Utiliza o GPIO12 (PWM0 channel 0) do Raspberry Pi 4
 * Created: 16/04/2026
 * Author : Ines Santos (1140623), André Pinto (1200209) e Gabriel Lopes (1252630)
 */
````

## Objetivo

Desenvolver um programa em user space capaz de configurar e controlar um sinal PWM no GPIO12 do Raspberry Pi 4 através de acesso direto aos registos de periféricos usando `/dev/mem`.

## Funcionamento

O programa:

* acede ao `/dev/mem`
* mapeia os registos do GPIO e do PWM
* configura o GPIO12 em ALT0
* ativa o PWM0 channel 0
* permite alterar o duty cycle no terminal sem recompilar

## GPIO

* Pino utilizado: **GPIO12**
* Ligação: GPIO12 → resistência (~220Ω) → LED → GND

## Compilação

```bash
make
```

## Execução

### Modo interativo

```bash
make run
```

### Com valor inicial por argumento

```bash
sudo ./pl4_ex2_pwm_user 128
```

## Teste

Durante a execução, o programa permite introduzir novos valores:

* `0` → LED desligado
* `64` → brilho reduzido
* `128` → brilho médio
* `255` → brilho máximo
* `-1` → sair do programa

## Validação

O programa foi testado no Raspberry Pi 4, verificando-se que o brilho do LED varia conforme o valor de duty cycle introduzido no terminal.

## Conclusão

Foi validado o controlo de PWM em user space, demonstrando:

* uso de `/dev/mem`
* acesso direto aos registos de GPIO e PWM
* controlo dinâmico do duty cycle sem kernel module

