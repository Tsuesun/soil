#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <gui/view.h>
#include <input/input.h>
#include <furi_hal_adc.h>

#define TAG "soil"

typedef struct {
    uint16_t adc_value;
    bool running;
} SoilAppState;

void soilcap_draw_callback(Canvas* canvas, void* context) {
    SoilAppState* state = (SoilAppState*)context;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_set_font(canvas, FontPrimary);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "ADC: %u", state->adc_value);
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, buffer);
}

void soilcap_input_callback(InputEvent* event, void* context) {
    SoilAppState* state = context;

    if(event->type == InputTypeShort && event->key == InputKeyBack) {
        state->running = false;
    }
}

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

    Gui* gui = furi_record_open("gui");
    ViewPort* view_port = view_port_alloc();
    SoilAppState state = {.adc_value = 0, .running = true};
    view_port_draw_callback_set(view_port, soilcap_draw_callback, &state);
    view_port_input_callback_set(view_port, soilcap_input_callback, &state);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(state.running) {
        state.adc_value = furi_hal_adc_read(adc_handle, FuriHalAdcChannel12);
        view_port_update(view_port);
        furi_delay_ms(1000);
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");

    furi_hal_adc_release(adc_handle);

    return 0;
}
