set(microsim_trigger_STAT_SRCS
   MSCalibrator.cpp
   MSCalibrator.h
   MSLaneSpeedTrigger.cpp
   MSLaneSpeedTrigger.h
   MSChargingStation.h
   MSChargingStation.cpp
   MSTriggeredRerouter.cpp
   MSTriggeredRerouter.h
   MSOverheadWire.cpp
   MSOverheadWire.h
   MSStoppingPlaceRerouter.h
   MSStoppingPlaceRerouter.cpp
)

add_library(microsim_trigger STATIC ${microsim_trigger_STAT_SRCS})
set_property(TARGET microsim_trigger PROPERTY PROJECT_LABEL "z_microsim_trigger")
