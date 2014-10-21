#include "WPILib.h"
#include "math.h"
#include "Vision/RGBImage.h"
#include "Vision/BinaryImage.h"
#include "CameraCode.h"

// CRE: Let's use the camera for something useful in 2015!!!
CameraCode::CameraCode(char *colorLED = "amber") {

	if (!strcmp(colorLED, "blue")) {
		// HSV threshold for BLUE: Hue is 130-180
		threshold = new Threshold(130, 180, 60, 255, 20, 255);
	} else if (!strcmp(colorLED, "amber")) {
		// HSV threshold for AMBER: Hue is 0-90
		threshold = new Threshold(0, 90, 127, 255, 127, 255);
	} else if (!strcmp(colorLED, "green")) {
		// HSV threshold for GREEN Hue is 60-100
		threshold = new Threshold(60, 100, 90, 255, 20, 255);
	} else {
		// WHITE
		threshold = new Threshold(0, 360, 90, 255, 20, 255);
	}
	cameraOut = new robotOut;
	cameraOut->printDebug("Getting instance of Axis camera object... \n", 2);
	camera = &AxisCamera::GetInstance("10.17.40.11");
	cameraOut->printDebug("Setting image brightness to 50%... \n", 2);
	camera->WriteBrightness(50);
	cameraOut->printDebug("done \n", 2);
	cameraOut->printDebug("Setting image resolution to 320x240... \n", 2);
	camera->WriteResolution(AxisCamera::kResolution_320x240);
	cameraOut->printDebug("done \n", 2);
	cameraOut->printDebug("Setting image compression to 75... ", 2);
	camera->WriteCompression(75);
	cameraOut->printDebug("done \n", 2);
}

void CameraCode::Test() {
	
	ParticleFilterCriteria2 criteria[] = {
			{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
	};												//Particle filter criteria, used to filter out small particles

	cameraOut->printDebug("Grabbing image from camera... \n", 2);
	//ColorImage *image;	// THIS DIDN'T WORK!! NEEDED TO USE HSLImage *
	HSLImage *image = camera->GetImage();
	cameraOut->printDebug("done \n", 2);
	cameraOut->printDebug("Writing raw image... \n", 2);
	image->Write("/raw_image.jpg");
	cameraOut->printDebug("done \n", 2);

	// get just the HSV filtered target pixels
	cameraOut->printDebug("Processing step 1: threshold... \n", 2);
	BinaryImage *thresholdImage = image->ThresholdHSV(*threshold);
	cameraOut->printDebug("Writing to flash... \n", 2);
	thresholdImage->Write("/threshold.bmp");
	cameraOut->printDebug("done \n", 2);

	// fill in partial and full rectangles
	cameraOut->printDebug("Processing step 2: filling rectangles... \n", 2);
	BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);  // fill in partial and full rectangles
	cameraOut->printDebug("Writing to flash... \n", 2);
	convexHullImage->Write("/ConvexHull.bmp");
	cameraOut->printDebug("done \n", 2);
	
	//Remove small particles
	BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);	//Remove small particles
	cameraOut->printDebug("Writing to flash... \n", 2);
	filteredImage->Write("/Filtered.bmp");
	cameraOut->printDebug("done\n", 3);

	// be sure to delete images after using them
	
	delete filteredImage;
	delete convexHullImage;
	delete thresholdImage;
	delete image;
}

/**
  * Do the image capture with the camera and apply the algorithm described above. This
  * sample will either get images from the camera or from an image file stored in the top
  * level directory in the flash memory on the cRIO. The file name in this case is "testImage.jpg"
  */

void CameraCode::targetImage() {
	
	ParticleFilterCriteria2 criteria[] = {
			{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
	};												//Particle filter criteria, used to filter out small particles

	cameraOut->printDebug("Grabbing image from camera... ", 2);
	HSLImage *image = camera->GetImage();				//To get the images from the camera comment the line above and uncomment this one
	cameraOut->printDebug("done\n", 3);
	
	// get just the HSV filtered target pixels
	cameraOut->printDebug("Processing step 1: threshold... ", 2);
	BinaryImage *thresholdImage = image->ThresholdHSV(*threshold);
	cameraOut->printDebug("done\n", 3);

	// fill in partial and full rectangles
	cameraOut->printDebug("Processing step 2: filling rectangles... ", 2);
	BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);
	cameraOut->printDebug("done\n", 3);

	//Remove small particles
	cameraOut->printDebug("Processing step 3: removing small particles... ", 2);
	BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);
	cameraOut->printDebug("done\n", 3);

	vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  //get a particle analysis report for each particle
	scores = new Scores[reports->size()];
	
	// fprintf(stderr,"reports->size = %d\n", reports->size());
	
	//Iterate through each particle, scoring it and determining whether it is a target or not
	for (unsigned i = 0; i < reports->size(); i++) {
	
		ParticleAnalysisReport *report = &(reports->at(i));
		
		scores[i].rectangularity = CameraCode::scoreRectangularity(report);
		scores[i].aspectRatioOuter = CameraCode::scoreAspectRatio(filteredImage, report, true);
		scores[i].aspectRatioInner = CameraCode::scoreAspectRatio(filteredImage, report, false);			
		scores[i].xEdge = CameraCode::scoreXEdge(thresholdImage, report);
		scores[i].yEdge = CameraCode::scoreYEdge(thresholdImage, report);
		char theOutput [64];
		if(CameraCode::scoreCompare(scores[i], false))
		{
			sprintf(theOutput, "particle: %d  is a High Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
			cameraOut->printDebug(theOutput, 2);
			sprintf(theOutput, "Distance: %f \n", CameraCode::computeDistance(thresholdImage, report, false));
			cameraOut->printDebug(theOutput, 2);
			cameraOut->printDebug("We found a high goal\n", 1);
		} else if (CameraCode::scoreCompare(scores[i], true)) {
			sprintf(theOutput, "particle: %d  is a Middle Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
			cameraOut->printDebug(theOutput, 2);
			sprintf(theOutput, "Distance: %f \n", CameraCode::computeDistance(thresholdImage, report, true));
			cameraOut->printDebug(theOutput, 2);
			cameraOut->printDebug("We found a middle goal\n", 1);
		} else {
			sprintf(theOutput, "particle: %d  is not a goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
			cameraOut->printDebug(theOutput, 2);
		}
		sprintf(theOutput, "rect: %f  ARinner: %f \n", scores[i].rectangularity, scores[i].aspectRatioInner);
		cameraOut->printDebug(theOutput, 2);
		sprintf(theOutput, "ARouter: %f  xEdge: %f  yEdge: %f  \n", scores[i].aspectRatioOuter, scores[i].xEdge, scores[i].yEdge);
		cameraOut->printDebug(theOutput, 2);
	}
	printf("\n");
	
	// be sure to delete images after using them
	
	delete filteredImage;
	delete convexHullImage;
	delete thresholdImage;
	delete image;
	
	//delete allocated reports and Scores objects also
	delete scores;
	delete reports;
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

	cameraOut->printDebug("attempting to compute distance", 2);
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
	
	// End of testing code
}


