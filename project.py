import cv2
from transformers import DetrImageProcessor, DetrForObjectDetection
import torch
from PIL import Image
import numpy as np
import os
os.environ["HF_HUB_DISABLE_SYMLINKS_WARNING"] = "1"

# Load the pre-trained model and processor
processor = DetrImageProcessor.from_pretrained("facebook/detr-resnet-50")
model = DetrForObjectDetection.from_pretrained("facebook/detr-resnet-50")

# OpenCV to capture video from the default camera (usually laptop's built-in camera)
cap = cv2.VideoCapture(0)

# Check if the camera is opened successfully
if not cap.isOpened():
    print("Error: Could not open camera.")
    exit()

print("Press 'q' to quit the application.")

while True:
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret:
        print("Error: Failed to capture frame.")
        break

    # Convert the frame to a PIL image (necessary for the processor)
    pil_image = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))

    # Preprocess the image
    inputs = processor(images=pil_image, return_tensors="pt")

    # Perform object detection
    with torch.no_grad():
        outputs = model(**inputs)

    # Post-process the results to get the bounding boxes, labels, and scores
    target_sizes = torch.tensor([pil_image.size[::-1]])  # (height, width)
    results = processor.post_process_object_detection(outputs, target_sizes=target_sizes, threshold=0.9)[0]

    # Draw the bounding boxes around detected persons
    for score, label, box in zip(results["scores"], results["labels"], results["boxes"]):
        if label.item() == 1:  # Label 1 corresponds to "person" in COCO dataset
            box = [round(i, 2) for i in box.tolist()]
            # Draw rectangle (box) on the frame
            cv2.rectangle(frame, (int(box[0]), int(box[1])), (int(box[2]), int(box[3])), (0, 255, 0), 2)
            # Add text label
            cv2.putText(frame, f"Person: {round(score.item(), 3)}", 
                        (int(box[0]), int(box[1]) - 10), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Display the resulting frame
    cv2.imshow('Person Detection', frame)

    # Break the loop if the user presses the 'q' key
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
