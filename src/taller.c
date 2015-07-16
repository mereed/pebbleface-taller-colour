/*
Copyright (C) 2015 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pebble.h"
#include "effect_layer.h"

static Window *window;

EffectLayer* effect_layer_inv;
EffectLayer* effect_layer_col_hr;
EffectLayer* effect_layer_col_min;

static AppSync sync;
static uint8_t sync_buffer[256];

static int blink;
static int invert;
static int bluetoothvibe;
static int hourlyvibe;
static int dayflip;
static int datesep;
static int hour_col;
static int min_col;

static bool appStarted = false;

enum {
  BLINK_KEY = 0x0,  //not used
  INVERT_KEY = 0x1,
  BLUETOOTHVIBE_KEY = 0x2,
  HOURLYVIBE_KEY = 0x3,
  DAYFLIP_KEY = 0x4,
  DATESEP_KEY = 0x5,
  HOUR_COL_KEY = 0x6,
  MIN_COL_KEY = 0x7
};

	// initializing colors
struct EffectColorpair {
  GColor firstColor;  // first color (target for colorize, one of set in colorswap)
  GColor secondColor; // second color (new color for colorize, other of set in colorswap)
};
  
EffectColorpair colorpair_a;
EffectColorpair colorpair_b;

static GBitmap *separator_image;
static BitmapLayer *separator_layer;

static GBitmap *bluetooth_image_on;
static BitmapLayer *bluetooth_layer_on;

static GBitmap *sep_mask_image;
static BitmapLayer *sep_mask;

static GBitmap *day_name_image;
static BitmapLayer *day_name_layer;

const int DAY_NAME_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DAY_NAME_SUN,
  RESOURCE_ID_IMAGE_DAY_NAME_MON,
  RESOURCE_ID_IMAGE_DAY_NAME_TUE,
  RESOURCE_ID_IMAGE_DAY_NAME_WED,
  RESOURCE_ID_IMAGE_DAY_NAME_THU,
  RESOURCE_ID_IMAGE_DAY_NAME_FRI,
  RESOURCE_ID_IMAGE_DAY_NAME_SAT
};

static GBitmap *day_name_image2;
static BitmapLayer *day_name_layer2;

const int DAY_NAME_IMAGE2_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DAY_NAME_SUN2,
  RESOURCE_ID_IMAGE_DAY_NAME_MON2,
  RESOURCE_ID_IMAGE_DAY_NAME_TUE2,
  RESOURCE_ID_IMAGE_DAY_NAME_WED2,
  RESOURCE_ID_IMAGE_DAY_NAME_THU2,
  RESOURCE_ID_IMAGE_DAY_NAME_FRI2,
  RESOURCE_ID_IMAGE_DAY_NAME_SAT2
};

#define TOTAL_DATE_DIGITS 2	
static GBitmap *date_digits_images[TOTAL_DATE_DIGITS];
static BitmapLayer *date_digits_layers[TOTAL_DATE_DIGITS];

const int DATENUM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DATENUM_0,
  RESOURCE_ID_IMAGE_DATENUM_1,
  RESOURCE_ID_IMAGE_DATENUM_2,
  RESOURCE_ID_IMAGE_DATENUM_3,
  RESOURCE_ID_IMAGE_DATENUM_4,
  RESOURCE_ID_IMAGE_DATENUM_5,
  RESOURCE_ID_IMAGE_DATENUM_6,
  RESOURCE_ID_IMAGE_DATENUM_7,
  RESOURCE_ID_IMAGE_DATENUM_8,
  RESOURCE_ID_IMAGE_DATENUM_9
};

#define TOTAL_DATE_DIGITS2 2	
static GBitmap *date_digits_images2[TOTAL_DATE_DIGITS2];
static BitmapLayer *date_digits_layers2[TOTAL_DATE_DIGITS2];

const int DATENUM_IMAGE2_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DATENUM_0B,
  RESOURCE_ID_IMAGE_DATENUM_1B,
  RESOURCE_ID_IMAGE_DATENUM_2B,
  RESOURCE_ID_IMAGE_DATENUM_3B,
  RESOURCE_ID_IMAGE_DATENUM_4B,
  RESOURCE_ID_IMAGE_DATENUM_5B,
  RESOURCE_ID_IMAGE_DATENUM_6B,
  RESOURCE_ID_IMAGE_DATENUM_7B,
  RESOURCE_ID_IMAGE_DATENUM_8B,
  RESOURCE_ID_IMAGE_DATENUM_9B
};

#define TOTAL_TIME_DIGITS 4
static GBitmap *time_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *time_digits_layers[TOTAL_TIME_DIGITS];

const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0,
  RESOURCE_ID_IMAGE_NUM_1,
  RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4,
  RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7,
  RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};



void change_hour() {
	  
    switch (hour_col) {
		

		case 0: //white		
			colorpair_a.firstColor = GColorWhite;		
  			colorpair_a.secondColor = GColorWhite;
		break;	
		
		case 1: //green	
  			colorpair_a.firstColor = GColorWhite;
  			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_a.secondColor = GColorGreen;
		#endif	
		break;

		case 2: //orange
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorOrange;
		#endif	
		break;

		case 3: //GColorCyan
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorCyan ;
		#endif	
		break;
		
		case 4: //yellow
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorYellow;
		#endif	
		break;
		
		case 5: //GColorSunsetOrange 
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorSunsetOrange;
		#endif	
		break;
		
		case 6: //GColorVividViolet 
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorVividViolet ;
		#endif	
		break;
		
		case 7: //GColorShockingPink  
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorShockingPink ;
		#endif	
		break;
		
		case 8: //GColorBrightGreen  
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorBrightGreen  ;
		#endif	
		break;

		case 9: //GColorElectricBlue 
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorElectricBlue ;
		#endif	
		break;
		
		case 10: //GColorChromeYellow 
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorChromeYellow ;
		#endif	
		break;
		
		case 11: //GColorDarkGray 
			colorpair_a.firstColor = GColorWhite;
			colorpair_a.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_a.secondColor = GColorDarkGray ;
		#endif	
		break; 
	}    
}

void change_min() {

    switch (min_col) {
		case 0: //white
  			colorpair_b.firstColor = GColorWhite;
  			colorpair_b.secondColor = GColorWhite;
		break;
		
		case 1: // green
  			colorpair_b.firstColor = GColorWhite;
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorGreen;
		#endif	
		break;

		case 2: //orange
  			colorpair_b.firstColor = GColorWhite;	
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorOrange;
		#endif	
		break;

		case 3: //GColorCyan 
  			colorpair_b.firstColor = GColorWhite;	
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorCyan ;
		#endif	
		break;	
		
		case 4: //GColorYellow  
  			colorpair_b.firstColor = GColorWhite;	
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorYellow  ;
		#endif	
		break;
		
		case 5: //GColorSunsetOrange  
  			colorpair_b.firstColor = GColorWhite;	
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorSunsetOrange  ;
		#endif	
		break;

		case 6: //GColorVividViolet  
  			colorpair_b.firstColor = GColorWhite;	
  			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
  			colorpair_b.secondColor = GColorVividViolet  ;
		#endif	
		break;

		case 7: //GColorShockingPink   
			colorpair_b.firstColor = GColorWhite;	
			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_b.secondColor = GColorShockingPink   ;
		#endif	
		break;
		
		case 8: //GColorBrightGreen    
			colorpair_b.firstColor = GColorWhite;	
			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_b.secondColor = GColorBrightGreen    ;
		#endif	
		break;
		
		case 9: //GColorElectricBlue    
			colorpair_b.firstColor = GColorWhite;	
			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_b.secondColor = GColorElectricBlue    ;
		#endif	
		break;
		
		case 10: //GColorChromeYellow   
			colorpair_b.firstColor = GColorWhite;	
			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_b.secondColor = GColorChromeYellow   ;
		#endif	
		break;
		
		case 11: //GColorDarkGray   
			colorpair_b.firstColor = GColorWhite;	
			colorpair_b.secondColor = GColorWhite;
		#ifdef PBL_COLOR		
			colorpair_b.secondColor = GColorDarkGray   ;
		#endif	
		break;
		
    }    
}

void change_background(bool invert) {
  if (invert && effect_layer_inv == NULL) {
    // Add inverter layer
    Layer *window_layer = window_get_root_layer(window);

    effect_layer_inv = effect_layer_create(GRect(0, 0, 144, 168));
    layer_add_child(window_layer, effect_layer_get_layer(effect_layer_inv));
    effect_layer_add_effect(effect_layer_inv, effect_invert, NULL);
	  
  } else if (!invert && effect_layer_inv != NULL) {
    // Remove Inverter layer
   layer_remove_from_parent(effect_layer_get_layer(effect_layer_inv));
   effect_layer_destroy(effect_layer_inv);
   effect_layer_inv = NULL;
  }
  // No action required
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed);

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case BLINK_KEY:
	  blink = new_tuple->value->uint8 !=0;
	  persist_write_bool(INVERT_KEY, blink);
      break;
	  
     case INVERT_KEY:
      invert = new_tuple->value->uint8 != 0;
	  persist_write_bool(INVERT_KEY, invert);
      change_background(invert);
      break;
	  
	case BLUETOOTHVIBE_KEY:
      bluetoothvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(BLUETOOTHVIBE_KEY, bluetoothvibe);
      break;      
	  
    case HOURLYVIBE_KEY:
      hourlyvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(HOURLYVIBE_KEY, hourlyvibe);	  
      break;	
	  	  
	case DATESEP_KEY:
      datesep = new_tuple->value->uint8 != 0;
	  persist_write_bool(DATESEP_KEY, datesep);	  
	  
	 if (datesep) {
		layer_set_hidden(bitmap_layer_get_layer(separator_layer), false);
			
	} else {
		layer_set_hidden(bitmap_layer_get_layer(separator_layer), true);

	}
      break;
	  
	case DAYFLIP_KEY:
      dayflip = new_tuple->value->uint8 != 0;
	  persist_write_bool(DAYFLIP_KEY, dayflip);	  
	  
	  	if (dayflip) {
		layer_set_hidden(bitmap_layer_get_layer(day_name_layer2), false);
		for (int i = 0; i < TOTAL_DATE_DIGITS2; ++i) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers2[i]), false);
		}
		
		layer_set_hidden(bitmap_layer_get_layer(day_name_layer), true);
		for (int i = 0; i < TOTAL_DATE_DIGITS; ++i) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[i]), true);
		}
				
	} else {
		
		layer_set_hidden(bitmap_layer_get_layer(day_name_layer2), true);
		for (int i = 0; i < TOTAL_DATE_DIGITS2; ++i) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers2[i]), true);
		}
		
		layer_set_hidden(bitmap_layer_get_layer(day_name_layer), false);
		for (int i = 0; i < TOTAL_DATE_DIGITS; ++i) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[i]), false);
		}

	}
      break;

    case HOUR_COL_KEY:
		hour_col = new_tuple->value->uint8;
		persist_write_bool(HOUR_COL_KEY, hour_col);
	    change_hour();
	    window_stack_push(window, true /* Animated */);

	break;
	  
    case MIN_COL_KEY:
		min_col = new_tuple->value->uint8;
		persist_write_bool(MIN_COL_KEY, min_col);
		change_min();
	  	window_stack_push(window, true /* Animated */);

	break;
  }
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
#ifdef PBL_PLATFORM_BASALT
  GRect bitmap_bounds = gbitmap_get_bounds((*bmp_image));
#else
  GRect bitmap_bounds = (*bmp_image)->bounds;
#endif
  GRect frame = GRect(origin.x, origin.y, bitmap_bounds.size.w, bitmap_bounds.size.h);
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}

static void toggle_bluetooth_icon(bool connected) {
	
  if(appStarted && !connected && bluetoothvibe) {
    //vibe!
    vibes_long_pulse();  
  }
	  layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer_on), connected);
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}

static void update_days(struct tm *tick_time) {
	
		set_container_image(&day_name_image2, day_name_layer2, DAY_NAME_IMAGE2_RESOURCE_IDS[tick_time->tm_wday], GPoint(66, 52));		
		set_container_image(&date_digits_images2[0], date_digits_layers2[0], DATENUM_IMAGE2_RESOURCE_IDS[tick_time->tm_mday/10], GPoint(66, 42));
  		set_container_image(&date_digits_images2[1], date_digits_layers2[1], DATENUM_IMAGE2_RESOURCE_IDS[tick_time->tm_mday%10], GPoint(66, 32));
		
		  set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[tick_time->tm_wday], GPoint(66, 25));
		  set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday/10], GPoint(66, 122));
   		  set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday%10], GPoint(66, 132));
	
}

static void update_hours(struct tm *tick_time) {

  if(appStarted && hourlyvibe) {
    //vibe!
    vibes_short_pulse();
  }
  
  unsigned short display_hour = get_display_hour(tick_time->tm_hour);

  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(5, 7));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(33, 7));

	if (!clock_is_24h_style()) {
    if (tick_time->tm_hour >= 12) {
	}
	
    if (display_hour/10 == 0) {
      layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), true);
    }
    else {
      layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), false);
    }
	}
  }

static void update_minutes(struct tm *tick_time) {
	
  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[tick_time->tm_min/10], GPoint(87, 7));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[tick_time->tm_min%10], GPoint(115, 7));
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  if (units_changed & DAY_UNIT) {
    update_days(tick_time);
  }
  if (units_changed & HOUR_UNIT) {
    update_hours(tick_time);
  }
  if (units_changed & MINUTE_UNIT) {
    update_minutes(tick_time);
  }	
		
}

static void init(void) {
  memset(&time_digits_layers, 0, sizeof(time_digits_layers));
  memset(&time_digits_images, 0, sizeof(time_digits_images));
  memset(&date_digits_layers, 0, sizeof(date_digits_layers));
  memset(&date_digits_images, 0, sizeof(date_digits_images));
  memset(&date_digits_layers2, 0, sizeof(date_digits_layers2));
  memset(&date_digits_images2, 0, sizeof(date_digits_images2));


  const int inbound_size = 256;
  const int outbound_size = 256;
  app_message_open(inbound_size, outbound_size);  

  window = window_create();
  if (window == NULL) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }
	
  window_set_background_color(window, GColorBlack);
		
  window_stack_push(window, true /* Animated */);
  Layer *window_layer = window_get_root_layer(window);
	
	// Create time and date layers
  GRect dummy_frame = { {0, 0}, {0, 0} };
   day_name_layer = bitmap_layer_create(dummy_frame);
   layer_add_child(window_layer, bitmap_layer_get_layer(day_name_layer));
	
	day_name_layer2 = bitmap_layer_create(dummy_frame);
   layer_add_child(window_layer, bitmap_layer_get_layer(day_name_layer2));	
	
  for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
    time_digits_layers[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(time_digits_layers[i]));
  }
	
  for (int i = 0; i < TOTAL_DATE_DIGITS; ++i) {
    date_digits_layers[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(date_digits_layers[i]));
  }

	for (int i = 0; i < TOTAL_DATE_DIGITS2; ++i) {
    date_digits_layers2[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(date_digits_layers2[i]));
  }
	
 sep_mask_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SEP_MASK);
#ifdef PBL_PLATFORM_BASALT
  GRect bitmap_bounds_mask = gbitmap_get_bounds(sep_mask_image);
#else
  GRect bitmap_bounds_mask = sep_mask_image->bounds;
#endif	
  GRect frame_mask = GRect(64, 10, bitmap_bounds_mask.size.w, bitmap_bounds_mask.size.h);
  sep_mask = bitmap_layer_create(frame_mask);
  bitmap_layer_set_bitmap(sep_mask, sep_mask_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(sep_mask));   
  layer_set_hidden(bitmap_layer_get_layer(sep_mask), true);
	
  separator_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SEPARATOR);
#ifdef PBL_PLATFORM_BASALT
  GRect bitmap_bounds = gbitmap_get_bounds(separator_image);
#else
  GRect bitmap_bounds = separator_image->bounds;
#endif	
  GRect frame = GRect(64, 10, bitmap_bounds.size.w, bitmap_bounds.size.h);
  separator_layer = bitmap_layer_create(frame);
  bitmap_layer_set_bitmap(separator_layer, separator_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer));   

   bluetooth_image_on = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTHON);
#ifdef PBL_PLATFORM_BASALT
  GRect bitmap_bounds_bt_on = gbitmap_get_bounds(bluetooth_image_on);
#else
  GRect bitmap_bounds_bt_on = bluetooth_image_on->bounds;
#endif	
  GRect frame_bton = GRect(64, 10, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
  bluetooth_layer_on = bitmap_layer_create(frame_bton);
  bitmap_layer_set_bitmap(bluetooth_layer_on, bluetooth_image_on);
  layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer_on));
	
	
  colorpair_a.firstColor = GColorWhite;
  colorpair_a.secondColor = GColorWhite;
  effect_layer_col_hr  = effect_layer_create(GRect(0,0,62,168));
  effect_layer_add_effect(effect_layer_col_hr, effect_colorize, &colorpair_a);
  layer_add_child(window_layer, effect_layer_get_layer(effect_layer_col_hr));

  colorpair_b.firstColor = GColorWhite;
  colorpair_b.secondColor = GColorWhite;
  effect_layer_col_min  = effect_layer_create(GRect(78,0,70,168));
  effect_layer_add_effect(effect_layer_col_min, effect_colorize, &colorpair_b);
  layer_add_child(window_layer, effect_layer_get_layer(effect_layer_col_min));
	

  toggle_bluetooth_icon(bluetooth_connection_service_peek());

  Tuplet initial_values[] = {
    TupletInteger(BLINK_KEY, persist_read_bool(BLINK_KEY)),
    TupletInteger(INVERT_KEY, persist_read_bool(INVERT_KEY)),
    TupletInteger(BLUETOOTHVIBE_KEY, persist_read_bool(BLUETOOTHVIBE_KEY)),
    TupletInteger(HOURLYVIBE_KEY, persist_read_bool(HOURLYVIBE_KEY)),
    TupletInteger(DAYFLIP_KEY, persist_read_bool(DAYFLIP_KEY)),
    TupletInteger(DATESEP_KEY, persist_read_bool(DATESEP_KEY)),
	TupletInteger(HOUR_COL_KEY, persist_read_bool(HOUR_COL_KEY)),
	TupletInteger(MIN_COL_KEY, persist_read_bool(MIN_COL_KEY)),
  };
  
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, NULL, NULL);
   
  appStarted = true;
  
  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT );

  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);

}


static void deinit(void) {
  app_sync_deinit(&sync);
  
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(separator_layer));
  bitmap_layer_destroy(separator_layer);
  gbitmap_destroy(separator_image);

  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer_on));
  bitmap_layer_destroy(bluetooth_layer_on);
  gbitmap_destroy(bluetooth_image_on);
	
  layer_remove_from_parent(bitmap_layer_get_layer(sep_mask));
  bitmap_layer_destroy(sep_mask);
  gbitmap_destroy(sep_mask_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(day_name_layer));
  bitmap_layer_destroy(day_name_layer);
  gbitmap_destroy(day_name_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(day_name_layer2));
  bitmap_layer_destroy(day_name_layer2);
  gbitmap_destroy(day_name_image2);
	
  for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(date_digits_layers[i]));
    gbitmap_destroy(date_digits_images[i]);
    bitmap_layer_destroy(date_digits_layers[i]);
  }

	 for (int i = 0; i < TOTAL_DATE_DIGITS2; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(date_digits_layers2[i]));
    gbitmap_destroy(date_digits_images2[i]);
    bitmap_layer_destroy(date_digits_layers2[i]);
  }
	
  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(time_digits_layers[i]));
    gbitmap_destroy(time_digits_images[i]);
    bitmap_layer_destroy(time_digits_layers[i]);
  }

  layer_remove_from_parent(effect_layer_get_layer(effect_layer_col_hr));
  effect_layer_destroy(effect_layer_col_hr);
  effect_layer_col_hr = NULL;
	
  layer_remove_from_parent(effect_layer_get_layer(effect_layer_col_min));
  effect_layer_destroy(effect_layer_col_min);
  effect_layer_col_min = NULL;

	
	  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}