###
## Version Information
###
set ( HPCC_PROJECT "community" )
set ( HPCC_MAJOR 7 )


set ( HPCC_MINOR 0 )
set ( HPCC_POINT 0 )
set ( HPCC_MATURITY "rc" )
set ( HPCC_SEQUENCE 5 )
###
IF ( ${HPCC_POINT} GREATER 65 )
  error ( "HPCC_POINT must be less than 66." )
ENDIF ( )
IF ( ${HPCC_SEQUENCE} GREATER 99 )
  error ( "HPCC_SEQUENCE must be less than 100." )
ENDIF ( )

#     HPCC_VERSION must be of the format 255.255.255.65535  ---
#     Samples:  6.10.2.001 (6.10.2-1rc)
#               6.10.2.101 (6.10.2-1)
#               6.10.2.102 (6.10.2-2)
#               6.10.23.045 (6.10.23-45rc)
#               6.10.67.189 (6.10.67-89)
IF ( "${HPCC_MATURITY}" STREQUAL "release" )
  SET ( HPCC_BUILD "1" )
ELSE ( )
  SET ( HPCC_BUILD "0" )
ENDIF ( )
IF ( ${HPCC_SEQUENCE} LESS 10 )
  SET ( HPCC_BUILD "${HPCC_BUILD}0${HPCC_SEQUENCE}" )
ELSE ( )
  SET ( HPCC_BUILD "${HPCC_BUILD}${HPCC_SEQUENCE}" )
ENDIF ( )
SET ( HPCC_VERSION ${HPCC_MAJOR}.${HPCC_MINOR}.${HPCC_POINT}.${HPCC_BUILD} )
MESSAGE ( "HPCC_VERSION:  ${HPCC_VERSION}" )

#     MSI_VERSION must be of the format 255.255.65535  ---
#     Samples:  6.10.2001 (6.10.2-1rc)
#               6.10.2101 (6.10.2-1)
#               6.10.2102 (6.10.2-2)
#               6.10.23045 (6.10.23-45rc)
#               6.10.67189 (6.10.67-89)
SET ( MSI_VERSION "${HPCC_MAJOR}.${HPCC_MINOR}.${HPCC_POINT}${HPCC_BUILD}" )
MESSAGE ( "MSI_VERSION:  ${MSI_VERSION}" )
###
