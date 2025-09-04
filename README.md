#Dataset — NB-IoT Urban Propagation (Santiago)

##Contextualization

This repository contains the measurement dataset and supporting code from a controlled NB-IoT field campaign conducted in a dense urban sector of Santiago, Chile (Band 28, 700 MHz). The study was designed to quantify height-dependent propagation effects for static IoT devices and to derive a height-dependent fine tuning of empirical path-loss models. Data were collected on an operational NB-IoT deployment using a stationary UE platform and a systematic sampling procedure to ensure statistical robustness and repeatability.

##Measurement sampling (points & samples)

Measurement points: 39 georeferenced locations distributed radially around the serving sector (systematic 25 m grid coverage within ≈450 m radius).

*Receiver heights: five nominal tripod heights — 0.1 m, 0.9 m, 1.5 m, 2.3 m, 3.0 m.
*Samples per location-height: 101 temporally stable samples recorded per (location, height) combination.
*Total samples: >19,000 RSRP samples across all locations and heights.

This sampling scheme isolates persistent spatial multipath / shadowing patterns (static UE) and produces per-point statistics (mean, std, percentiles) used in the analysis.

##KPIs extracted per measurement

Each recorded sample includes radio KPIs, geospatial metadata and device/environmental metadata. Typical CSV headers are:
*cell_id, tac: serving cell identifiers.
*rsrp_dbm, rssi_dbm, rsrq_db, sinr_db: radio measures for link budgeting and quality.
*ce_level: NB-IoT Coverage Enhancement level.
*ber: bit-error indicator recorded during tests.
*temperature_c, humidity_pct: local environmental readings from onboard sensor.

##User Equipment (UE) — hardware & firmware

Measurement UE (NB-Kit) summary:
*Core modules: RAK WisBlock stack — RAK11200 (ESP32) microcontroller + Quectel BG77 NB-IoT modem.
*Power & storage: LiPo battery and SD card logging (RAK15002 or equivalent).
*Positioning & sensors: active GNSS antenna for geotags and onboard environmental sensor (temperature, humidity).
*Peripherals: OLED status display and monopole LTE antenna (≈5 dBi).
*Firmware: implemented as a PlatformIO (VSCode) project. Firmware orchestrates modem AT commands, GNSS reads, SD logging and CSV formatting.

##Data collection algorithm 

<img width="1562" height="515" alt="Algorithm (1)" src="https://github.com/user-attachments/assets/d323565b-abb7-4a4b-a5e3-0da1c2098faf" />
