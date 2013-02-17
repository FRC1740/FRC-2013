#include "WPILib.h"
#include "math.h"
#include "Vision/RGBImage.h"
#include "Vision/BinaryImage.h"
#include "CameraCode.h"

CameraCode::CameraCode() {
	fprintf(stderr, "Getting instance of Axis camera object... ");
	camera = &AxisCamera::GetInstance("10.17.40.11");
	fprintf(stderr, "done.\n");
}

void CameraCode::Test() {
	
	// Configure Threshold to recognize AMBER
	Threshold threshold(0, 90, 127, 255, 127, 255);	//HSV threshold criteria, ranges are in that order ie. Hue is 60-100
/*
	ParticleFilterCriteria2 criteria[] = {
			{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
	};												//Particle filter criteria, used to filter out small particles
*/
	//ColorImage *image;
	fprintf(stderr, "Grabbing image from camera... ");
	camera->WriteBrightness(50);
	camera->WriteResolution(AxisCamera::kResolution_320x240);
	camera->WriteCompression(80);
	HSLImage *image = camera->GetImage();				//To get the images from the camera comment the line above and uncomment this one
	fprintf(stderr, "done.\n");
	fprintf(stderr, "Writing raw image... ");
	image->Write("/raw_image.jpg");
	fprintf(stderr, "done.\n");
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
double CameraCode::scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report, bool outer){
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
double CameraCode::computeDistance (BinaryImage *image, ParticleAnalysisReport *report, bool outer) {
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
 * Compares scores to defined limits and returns true if the particle appears to be a target
 * 
 * @param scores The structure containing the scores to compare
 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
 * 
 * @return True if the particle meets all limits, false otherwise
 */
bool CameraCode::scoreCompare(Scores scores, bool outer){
	bool isTarget = true;

	isTarget &= scores.rectangularity > RECTANGULARITY_LIMIT;
	if(outer){
		isTarget &= scores.aspectRatioOuter > ASPECT_RATIO_LIMIT;
	} else {
		isTarget &= scores.aspectRatioInner > ASPECT_RATIO_LIMIT;
	}
	isTarget &= scores.xEdge > X_EDGE_LIMIT;
	isTarget &= scores.yEdge > Y_EDGE_LIMIT;

	return isTarget;
}
/**
 * Computes a score (0-100) estimating how rectangular the particle is by comparing the area of the particle
 * to the area of the bounding box surrounding it. A perfect rectangle would cover the entire bounding box.
 * 
 * @param report The Particle Analysis Report for the particle to score
 * @return The rectangularity score (0-100)
 */
double CameraCode::scoreRectangularity(ParticleAnalysisReport *report){
	if(report->boundingRect.width*report->boundingRect.height !=0){
		return 100*report->particleArea/(report->boundingRect.width*report->boundingRect.height);
	} else {
		return 0;
	}	
}

/**
 * Computes a score based on the match between a template profile and the particle profile in the X direction. This method uses the
 * the column averages and the profile defined at the top of the sample to look for the solid vertical edges with
 * a hollow center.
 * 
 * @param image The image to use, should be the image before the convex hull is performed
 * @param report The Particle Analysis Report for the particle
 * 
 * @return The X Edge Score (0-100)
 */
double CameraCode::scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report){
	double total = 0;
	LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_COLUMN_AVERAGES, report->boundingRect);
	for(int i=0; i < (averages->columnCount); i++){
		if(xMin[i*(XMINSIZE-1)/averages->columnCount] < averages->columnAverages[i] 
		                                                                         && averages->columnAverages[i] < xMax[i*(XMAXSIZE-1)/averages->columnCount]){
			total++;
		}
	}
	total = 100*total/(averages->columnCount);		//convert to score 0-100
	imaqDispose(averages);							//let IMAQ dispose of the averages struct
	return total;
}

/**
 * Computes a score based on the match between a template profile and the particle profile in the Y direction. This method uses the
 * the row averages and the profile defined at the top of the sample to look for the solid horizontal edges with
 * a hollow center
 * 
 * @param image The image to use, should be the image before the convex hull is performed
 * @param report The Particle Analysis Report for the particle
 * 
 * @return The Y Edge score (0-100)
 */
double CameraCode::scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report){
	double total = 0;
	LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_ROW_AVERAGES, report->boundingRect);
	for(int i=0; i < (averages->rowCount); i++){
		if(yMin[i*(YMINSIZE-1)/averages->rowCount] < averages->rowAverages[i] 
		                                                                   && averages->rowAverages[i] < yMax[i*(YMAXSIZE-1)/averages->rowCount]){
			total++;
		}
	}
	total = 100*total/(averages->rowCount);		//convert to score 0-100
	imaqDispose(averages);						//let IMAQ dispose of the averages struct
	return total;
}

void CameraCode::ReadProcessWrite() {
	
	// The following HSV threshold recognizes GREEN 
	//		Threshold threshold(60, 100, 90, 255, 20, 255);	//HSV threshold criteria, ranges are in that order ie. Hue is 60-100

	// Attempting to recognize BLUE 
	//		Threshold threshold(130, 180, 60, 255, 20, 255);	//HSV threshold criteria, ranges are in that order ie. Hue is 60-100

	// Attempting to recognize AMBER
	Threshold threshold(0, 90, 127, 255, 127, 255);	//HSV threshold criteria, ranges are in that order ie. Hue is 60-100
	ParticleFilterCriteria2 criteria[] = {
			{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
	};												//Particle filter criteria, used to filter out small particles

	//		SmartDashboard::PutBoolean("In Teleop", false);

	// The next group of lines are for testing image capture from the Axis 1011
	printf("initalizing camera\n");
	//		AxisCamera &camera = AxisCamera::GetInstance();	//To use the Axis camera uncomment this line
	ColorImage *image;
	image = new RGBImage("/amber_image.jpg");		// get the sample image from the cRIO flash
	//camera.GetImage(image);				//To get the images from the camera comment the line above and uncomment this one
	fprintf(stderr,"Writing raw image... ");
	image->Write("/raw_image.jpg");
	fprintf(stderr, "done.\n");
	BinaryImage *thresholdImage = image->ThresholdHSV(threshold);	// get just the (green/blue/etc) target pixels
	fprintf(stderr,"Writing threshold image... ");
	thresholdImage->Write("/threshold.bmp");
	fprintf(stderr, "done.\n");
	BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);  // fill in partial and full rectangles
	fprintf(stderr,"Writing convex image... ");
	convexHullImage->Write("/ConvexHull.bmp");
	fprintf(stderr, "Done.\n");
	BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);	//Remove small particles
	fprintf(stderr,"Writing filtered image... ");
	filteredImage->Write("Filtered.bmp");
	fprintf(stderr, "done.\n");
	delete image;
	// End of testing code
}


