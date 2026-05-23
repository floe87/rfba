#ifndef _sgp4_sgp4Propagator_
#define _sgp4_sgp4Propagator_

#include "../omm/omm.h"
#include "coordinates.h"


/**
 * @brief This class implements the SGP4 orbit propagator
 *
 * @author Andreas Maurer, andreas.maurer@uni-wuerzburg.de
 */
class SGP4Propagator
{
private:
	OMM omm; //< contains the OMM used for the orbit propagation
	bool neConstsInitialized = false; ///< indicates if the near earth constants were initialized

public:
	SGP4Propagator();

	/**
	 * @brief Sets the OMM used for the orbit propagation with the SGP4 model
	 *
	 * This function is also used to update the OMM once a new one is available.
	 */
	inline void setOmm(const OMM &_omm)
	{
		//set the OMM to the new value
		this->omm = _omm;

		//mark initialization of near earth constant as NOT done
		neConstsInitialized = false;
	}

	/**
	 * @return true if the NearEarthConstant are initialized
	 */
	inline bool areNeConstsInitialized() const
	{
		return neConstsInitialized;
	}

	/**
	 * @brief Calculates the position and velocity of the satellite in the ECI coordinate frame using the SGP4 model
	 *
	 * @param secsAfterEpoch	Seconds after the epoch of the OMM [s]
	 * @param satPos			Position of the satellite in ECI [km]
	 * @param satVel			Velocity of the satellite in ECI [km/s]
	 */
	void calculatePositionAndVelocity(int32_t secsAfterEpoch, ECICoordinate &satPos, ECICoordinate &satVel);
};

#endif /* end _sgp4_sgp4Propagator_ */

