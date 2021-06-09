# string detector_types[] = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"};
# string descriptor_types[] = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};

for DETECTOR in "SHITOMASI" "HARRIS" "FAST" "BRISK" "ORB" "AKAZE" "SIFT"
do
	for DESCRIPTOR in "BRISK" "BRIEF" "ORB" "FREAK" "AKAZE" "SIFT"
    do
        ./3D_object_tracking $DETECTOR $DESCRIPTOR NO_GRAPHICS
    done
done