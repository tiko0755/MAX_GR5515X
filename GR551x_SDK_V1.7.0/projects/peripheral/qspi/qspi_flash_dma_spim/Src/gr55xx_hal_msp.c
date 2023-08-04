/**
 *****************************************************************************************
 *
 * @file gr55xx_hal_msp.c
 *
 * @brief HAL MSP module.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gr55xx_hal.h"
#include "boards.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#define QSPI_GET_IRQNUM(__QSPIx__)    (((__QSPIx__) == (QSPI0)) ? QSPI0_IRQn : QSPI1_IRQn)

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static dma_handle_t s_dma_handle;

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void hal_qspi_msp_init(qspi_handle_t *p_qspi)
{
    gpio_init_t gpio_config = GPIO_DEFAULT_CONFIG;

    /* Config GPIO of QSPI */
    gpio_config.mode = GPIO_MODE_MUX;
    gpio_config.mux  = QSPI_GPIO_MUX;
    gpio_config.pin  = QSPI_CS_PIN | QSPI_CLK_PIN | QSPI_IO0_PIN | QSPI_IO1_PIN | QSPI_IO2_PIN | QSPI_IO3_PIN;
    hal_gpio_init(QSPI_GPIO_PORT, &gpio_config);

    __HAL_LINKDMA(p_qspi, p_dma, s_dma_handle);
    /* Configure the DMA handler for Transmission process */
    p_qspi->p_dma->channel                 = DMA_Channel0;
    p_qspi->p_dma->init.direction           = DMA_PERIPH_TO_PERIPH;
    p_qspi->p_dma->init.src_request         = (p_qspi->p_instance == QSPI0) ? DMA_REQUEST_QSPI0_RX : DMA_REQUEST_QSPI1_RX;
    p_qspi->p_dma->init.dst_request         = DMA_REQUEST_SPIM_TX;
    p_qspi->p_dma->init.src_increment       = DMA_SRC_NO_CHANGE;
    p_qspi->p_dma->init.dst_increment       = DMA_DST_NO_CHANGE;
    p_qspi->p_dma->init.src_data_alignment  = DMA_SDATAALIGN_WORD;
    p_qspi->p_dma->init.dst_data_alignment  = DMA_DDATAALIGN_WORD;
    p_qspi->p_dma->init.mode                = DMA_NORMAL;
    p_qspi->p_dma->init.priority            = DMA_PRIORITY_LOW;
    hal_dma_deinit(p_qspi->p_dma);
    hal_dma_init(p_qspi->p_dma);

    NVIC_ClearPendingIRQ(QSPI_GET_IRQNUM(p_qspi->p_instance));
    NVIC_EnableIRQ(QSPI_GET_IRQNUM(p_qspi->p_instance));
}

void hal_qspi_msp_deinit(qspi_handle_t *p_qspi)
{
    hal_gpio_deinit(QSPI_GPIO_PORT, QSPI_CS_PIN | QSPI_CLK_PIN | QSPI_IO0_PIN | QSPI_IO1_PIN | QSPI_IO2_PIN | QSPI_IO3_PIN);
    hal_dma_deinit(p_qspi->p_dma);
    NVIC_DisableIRQ(QSPI_GET_IRQNUM(p_qspi->p_instance));
}

void hal_spi_msp_init(spi_handle_t *p_spi)
{
    gpio_init_t gpio_config = GPIO_DEFAULT_CONFIG;

    gpio_config.mode = GPIO_MODE_MUX;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.pin  = SPIM_CS0_PIN | SPIM_CLK_PIN | SPIM_MOSI_PIN | SPIM_MISO_PIN;
    gpio_config.mux  = SPIM_GPIO_MUX;
    hal_gpio_init(SPIM_GPIO_PORT, &gpio_config);
}

void hal_spi_msp_deinit(spi_handle_t *p_spi)
{
    hal_gpio_deinit(SPIM_GPIO_PORT, SPIM_CS0_PIN | SPIM_CLK_PIN | SPIM_MOSI_PIN | SPIM_MISO_PIN);
}
