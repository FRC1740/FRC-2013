#include "WPILib.h"
#include "Math.h"
#include "CameraCode.h"

class cameraCode {
public:
	/**
	 * Computes the estimated distance to a target using the height of the particle in the image. For more information and graphics
	 * showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
	 * 
	 * @param image The image to use for measuring the particle estimated rectangle
	 * @param report The Particle Analysis Report for the particle
	 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
	 * @return The estimated distance to the target in Inches.
	 */
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, bool outer) {
		double rectShort, height;
		int targetHeight;

		printf("attempting to compute distance");
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
		//using the smaller of the estimated rectangle short side and the bounding rectangle height results in better performance
		//on skewed rectangles
		height = min(report->boundingRect.height, rectShort);
		targetHeight = outer ? 29 : 21;

		return X_IMAGE_RES * targetHeight / (height * 12 * 2 * tan(VIEW_ANGLE*PI/(180*2)));
	}
	/**
	 * Computes the estimated distance to a target using the height of the particle in the image. For more information and graphics
	 * showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
	 * 
	 * @param image The image to use for measuring the particle estimated rectangle
	 * @param report The Particle Analysis Report for the particle
	 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
	 * @return The estimated distance to the target in Inches.
	 */
	
	/**
	 * Computes a score (0-100) comparing the aspect ratio to the ideal aspect ratio for the target. This method uses
	 * the equivalent rectangle sides to determine aspect ratio as it performs better as the target gets skewed by moving
	 * to the left or right. The equivalent rectangle is the rectangle with sides x and y where particle area= x*y
	 * and particle perimeter= 2x+2y
	 * 
	 * @param image The image containing the particle to score, needed to perform additional measurements
	 * @param report The Particle Analysis Report for the particle, used for the width, height, and particle number
	 * @param outer	Indicates whether the particle aspect ratio should be compared to the ratio for the inner target or the outer
	 * @return The aspect ratio score (0-100)
	 */
	double scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report, bool outer){
		double rectLong, rectShort, idealAspectRatio, aspectRatio;
		idealAspectRatio = outer ? (62/29) : (62/20);	//Dimensions of goal opening + 4 inches on all 4 sides for reflective tape
		
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_LONG_SIDE, &rectLong);
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
		
		//Divide width by height to measure aspect ratio
		if(report->boundingRect.width > report->boundingRect.height){
			//particle is wider than it is tall, divide long by short
			aspectRatio = 100*(1-fabs((1-((rectLong/rectShort)/idealAspectRatio))));
		} else {
			//particle is taller than it is wide, divide short by long
			aspectRatio = 100*(1-fabs((1-((rectShort/rectLong)/idealAspectRatio))));
		}
		return (max(0, min(aspectRatio, 100)));		//force to be in range 0-100
	}
};
