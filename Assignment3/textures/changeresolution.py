import cv2

img1 = cv2.imread("brickwall.jpg")
img1 = cv2.resize(img1,(512,512))
img2 = cv2.imread("brickwall_normal.jpg")
img2 = cv2.resize(img2,(512,512))
cv2.imwrite("brickwall1.jpg",img1)
cv2.imwrite("brickwall1_normal.jpg",img2)
