/*
 * usb_conf.h
 *
 *  Created on: 18 août 2015
 *      Author: S. Poiraud
 */

#ifndef APPLI_USB_CONF_H_
#define APPLI_USB_CONF_H_



// On passe en mode USB HOST (accueil de périphériques USB)
#define USE_HOST_MODE
//#define USE_DEVICE_MODE

#ifdef USE_HOST_MODE
#ifdef USE_DEVICE_MODE
#error "You MUST NOT define HOST and DEVICE mode... A choice MUST be done!"
#endif
#endif


#define USE_USB_OTG_FS

#endif /* APPLI_USB_CONF_H_ */
