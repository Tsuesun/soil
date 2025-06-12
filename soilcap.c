#include <furi.h>
#include <furi_hal.h>

#define TAG "soil"

/** entrypoint */
int32_t soilcap_app(void* p) {
    UNUSED(p);

    FuriHalAdcHandle* adc_handle = furi_hal_adc_acquire();
    furi_hal_adc_configure_ex(
        adc_handle,
        FuriHalAdcScale2048,
        FuriHalAdcClockSync64,
        FuriHalAdcOversample64,
        FuriHalAdcSamplingtime247_5);

    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);

    for(int i = 0; i < 60; i++) {
        uint16_t adc_value = furi_hal_adc_read(adc_handle, FuriHalAdcChannel12);
        float millivolts = furi_hal_adc_convert_to_voltage(adc_handle, adc_value);
        FURI_LOG_D(TAG, "ADC value: %d Voltage: %f mV", adc_value, (double)millivolts);
        furi_delay_ms(1000);
    }

    furi_hal_adc_release(adc_handle);

    return 0;
}
