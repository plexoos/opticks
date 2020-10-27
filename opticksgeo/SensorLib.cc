#include "PLOG.hh"
#include "NPY.hpp"
#include "SensorLib.hh"

const plog::Severity SensorLib::LEVEL = PLOG::EnvLevel("SensorLib", "DEBUG"); 

const char* SensorLib::SENSOR_DATA = "sensorData.npy" ;
const char* SensorLib::SENSOR_ANGULAR_EFFICIENCY = "angularEfficiency.npy" ;

SensorLib* SensorLib::Load(const char* dir)  // static 
{
    LOG(info) << dir ; 
    return new SensorLib(dir) ; 
}

SensorLib::SensorLib(const char* dir)
    :
    m_loaded(dir ? true : false),
    m_sensor_data(m_loaded ? NPY<float>::load(dir, SENSOR_DATA) :  NULL),
    m_sensor_num(m_loaded ? m_sensor_data->getNumItems() : 0 ),
    m_sensor_angular_efficiency(m_loaded ? NPY<float>::load(dir, SENSOR_ANGULAR_EFFICIENCY) : NULL)
{
    LOG(LEVEL);
}

void SensorLib::save(const char* dir) const 
{
    LOG(info) << dir ; 
    if(m_sensor_data != NULL)
        m_sensor_data->save(dir, SENSOR_DATA); 

    if(m_sensor_angular_efficiency != NULL)
        m_sensor_angular_efficiency->save(dir, SENSOR_ANGULAR_EFFICIENCY );
}


void SensorLib::initSensorData(unsigned sensor_num)
{
    assert( ! m_loaded ) ; 
    LOG(LEVEL) << " sensor_num " << sensor_num  ;
    m_sensor_num = sensor_num ;  
    m_sensor_data = NPY<float>::make(m_sensor_num, 4);  
    m_sensor_data->zero(); 
}

/**
SensorLib::setSensorData
---------------------------

Calls to this for all sensor_placements G4PVPlacement provided by SensorLib::getSensorPlacements
provides a way to associate the Opticks contiguous 0-based sensorIndex with a detector 
defined sensor identifier. 

Within JUNO simulation framework this is used from LSExpDetectorConstruction::SetupOpticks.

sensorIndex 
    0-based continguous index used to access the sensor data, 
    the index must be less than the number of sensors
efficiency_1 
efficiency_2
    two efficiencies which are multiplied together with the local angle dependent efficiency 
    to yield the detection efficiency used to assign SURFACE_COLLECT to photon hits 
    that already have SURFACE_DETECT 
category
    used to distinguish between sensors with different theta textures   
identifier
    detector specific integer representing a sensor, does not need to be contiguous


**/

void SensorLib::setSensorData(unsigned sensorIndex, float efficiency_1, float efficiency_2, int category, int identifier)
{
    assert( sensorIndex < m_sensor_num );
    m_sensor_data->setFloat(sensorIndex,0,0,0, efficiency_1);
    m_sensor_data->setFloat(sensorIndex,1,0,0, efficiency_2);
    m_sensor_data->setInt(  sensorIndex,2,0,0, category);
    m_sensor_data->setInt(  sensorIndex,3,0,0, identifier);
}


void SensorLib::getSensorData(unsigned sensorIndex, float& efficiency_1, float& efficiency_2, int& category, int& identifier) const
{   
    assert( sensorIndex < m_sensor_num ); 
    assert( m_sensor_data );
    efficiency_1 = m_sensor_data->getFloat(sensorIndex,0,0,0);
    efficiency_2 = m_sensor_data->getFloat(sensorIndex,1,0,0);
    category = m_sensor_data->getInt(sensorIndex,2,0,0);
    identifier = m_sensor_data->getInt(sensorIndex,3,0,0);
}

int SensorLib::getSensorIdentifier(unsigned sensorIndex) const
{
    assert( sensorIndex < m_sensor_num );
    assert( m_sensor_data );
    return m_sensor_data->getInt( sensorIndex, 3, 0, 0);
}

/*
template <typename T>
void SensorLib::setSensorDataMeta( const char* key, T value )
{
    assert( m_sensor_data );
    m_sensor_data->setMeta<T>( key, value );
}
*/

void SensorLib::setSensorAngularEfficiency( 
        const std::vector<int>& shape, 
        const std::vector<float>& values,
        int theta_steps, float theta_min, float theta_max,
        int phi_steps,   float phi_min, float phi_max )
{
    LOG(LEVEL) << "[" ;
    const NPY<float>* a = MakeSensorAngularEfficiency(shape, values, theta_steps, theta_min, theta_max, phi_steps, phi_min, phi_max) ;
    setSensorAngularEfficiency(a);
    LOG(LEVEL) << "]" ;
}

const NPY<float>*  SensorLib::MakeSensorAngularEfficiency(        // static 
          const std::vector<int>& shape, 
          const std::vector<float>& values,
          int theta_steps, float theta_min, float theta_max,  
          int phi_steps,   float phi_min, float phi_max )   
{
    std::string metadata = "" ;
    NPY<float>* a = new NPY<float>(shape, values, metadata);
    a->setMeta<int>("theta_steps", theta_steps);
    a->setMeta<float>("theta_min", theta_min);
    a->setMeta<float>("theta_max", theta_max);
    a->setMeta<int>("phi_steps", phi_steps);
    a->setMeta<float>("phi_min", phi_min);
    a->setMeta<float>("phi_max", phi_max);
    return a ;
}


void SensorLib::setSensorAngularEfficiency( const NPY<float>* sensor_angular_efficiency )
{
    m_sensor_angular_efficiency = sensor_angular_efficiency ;
}






/*
NPY<float>*  SensorLib::getSensorDataArray() const
{
    return m_sensor_data ;
}
const NPY<float>*  SensorLib::getSensorAngularEfficiencyArray() const
{
    return m_sensor_angular_efficiency ;
}
template <typename T>
void SensorLib::setSensorAngularEfficiencyMeta( const char* key, T value )
{
    assert( m_sensor_angular_efficiency ); 
    m_sensor_angular_efficiency->setMeta<T>( key, value ); 
}

template OKGEO_API void SensorLib::setSensorDataMeta(const char* key, int value);
template OKGEO_API void SensorLib::setSensorDataMeta(const char* key, float value);
template OKGEO_API void SensorLib::setSensorDataMeta(const char* key, std::string value);

template OKGEO_API void SensorLib::setSensorAngularEfficiencyMeta(const char* key, int value);
template OKGEO_API void SensorLib::setSensorAngularEfficiencyMeta(const char* key, float value);
template OKGEO_API void SensorLib::setSensorAngularEfficiencyMeta(const char* key, std::string value);
*/


