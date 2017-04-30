//--------Modificado por Pablo Liberna, pliberona@austral-robotics.com / 29-10-2015

/*Version 1.5: Streaming estable, no se congela por el filtrado de areas de la linea 595
el thresholding se hace para escala de grises... más simple pero más efectivo
El cambio de mapa de color se hace a GRAY y ya no a HSV ya que en HSV solo influía el canal H
*/

#define _CRT_SECURE_NO_DEPRECATE//Ignora error C4996 "fopen"
#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <stdio.h>
//#include <cv.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <time.h>
#include <windows.h>
#include <direct.h>

//HEADERS OBSOLETOS
//#include <conio.h>
#include <sstream>
#include <string>
//#include <initializer_list>
//#include <iterator>


using namespace cv;
using namespace std;

GUID uuid;//Direccion MAC
ofstream myfile;//Crea archivo txt 

//Variables del programa NO MODIFICAR
int distanciareco = 0;
int estado = 0;
int posX = 0;
int posY = 0;
int iLastX = -1;
int iLastY = -1;
char centrotxt[50];
char centro[50];
char distancia[50];
char AreaChar[20];
char mac_addr[18];
int largest_area = 0;
int largest_contour_index = 0;
Rect bounding_rect;

//Objetos para hacer calculo de contornos
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Point> approx;

//Variables por defecto, modificables. 14 variables más 4 que son los flags de las ventanas.
int areaMenor = 500;
int areaMayor = 3e3;
int Vertice1_x = 58, Vertice2_x = 280, Vertice3_x = 212; //Vértices triángulo ROI
int Vertice1_y = 43, Vertice2_y = 40, Vertice3_y = 105;  //Vértices triángulo ROI
int Borde_Izq = 20, Borde_Der = 300, Borde_Sup = 57, Borde_Inf = 150; //Bordes del cuadro de la jaula
int Linea_ = 105; //Linea que divide y define si el raton esta parado o sentado
int Umbral_ = 150; //Valor del threshold predeterminado para filtrar
bool flipCamara = false, flipParameters = false, flipCamaraUmbral = false, flipFile = false; //Flags de las ventanas

//OBSOLETO DESDE EL 22-02-16
//========
//char text[255];
//char cantcontornos[50];
//int Hmin = 0, Smin = 0, Vmin = 107;
//int Hmax = 180, Smax = 256, Vmax = 256;

namespace{

	/*
	void DireccionMAC(GUID uuid, char mac_addr[18])
	{

	CoCreateGuid(&uuid);
	sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
	uuid.Data4[2], uuid.Data4[3], uuid.Data4[4],
	uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);
	}
	*/

	void Ventanas()
	{
		//namedWindow("AntesDeEqualizar");
		//moveWindow("AntesDeEqualizar", 680, 460);

		//namedWindow("DespuesDeEqualizar");
		//moveWindow("DespuesDeEqualizar", 340, 300);

		//namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
		//moveWindow("calcHist Demo", 700, 400);
	}

	const string currentDateTime()//FUNCION 1: TIMESTAMP
	{
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
//		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
		return buf;
	}
	
	/*
	Mat graficarHistograma(Mat& src)
	{    /// Separate the image in 3 places ( B, G and R )
		vector<Mat> bgr_planes;
		split(src, bgr_planes);

		/// Establish the number of bins
		int histSize = 256;

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 };
		const float* histRange = { range };

		bool uniform = true; bool accumulate = false;

		Mat b_hist, g_hist, r_hist;

		/// Compute the histograms:
		calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

		// Draw the histograms for B, G and R
		int hist_w = 512; int hist_h = 200;
		int bin_w = cvRound((double)hist_w / histSize);

		Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		/// Normalize the result to [ 0, histImage.rows ]
		normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

		/// Draw for each channel
		for (int i = 1; i < histSize; i++)
		{
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
		}
		/// Display
		imshow("calcHist Demo", histImage);

		return histImage;
	}*/

	Mat equalizarHistograma(const Mat& inputImage)//FUNCION 4: EQUALIZACION HISTOGRAMA
	{
		if (inputImage.channels() >= 3)
		{
			Mat ycrcb;

			cvtColor(inputImage, ycrcb, CV_BGR2YCrCb);

			vector<Mat> channels;
			split(ycrcb, channels);

			equalizeHist(channels[0], channels[0]);

			Mat result;
			merge(channels, ycrcb);

			cvtColor(ycrcb, result, CV_YCrCb2BGR);
			//imshow("DespuesDeEqualizar", inputImage);
			return result;
		}
		return Mat();
	}


	void setwindowsettings(){
		namedWindow("Parametros", CV_WINDOW_NORMAL);
				createTrackbar("AreaMin", "Parametros", &areaMenor, 4e3, NULL);
				createTrackbar("AreaMax", "Parametros", &areaMayor, 4e4, NULL);

				createTrackbar("Linea", "Parametros", &Linea_, 300, NULL);

				createTrackbar("Umbral", "Parametros", &Umbral_, 255, NULL);

				createTrackbar("Vertice1x", "Parametros", &Vertice1_x, 300, NULL);
				createTrackbar("Vertice1y", "Parametros", &Vertice1_y, 300, NULL);

				createTrackbar("Vertice2x", "Parametros", &Vertice2_x, 300, NULL);
				createTrackbar("Vertice2y", "Parametros", &Vertice2_y, 300, NULL);

				createTrackbar("Vertice3x", "Parametros", &Vertice3_x, 300, NULL);
				createTrackbar("Vertice3y", "Parametros", &Vertice3_y, 300, NULL);

				//Valores maximos de trackbars de bordes definidos según imagen a trabajar 320x240 pix
				createTrackbar("BordeIzq", "Parametros", &Borde_Izq, 320, NULL);
				createTrackbar("BordeDer", "Parametros", &Borde_Der, 320, NULL);
				createTrackbar("BordeSup", "Parametros", &Borde_Sup, 240, NULL);
				createTrackbar("BordeInf", "Parametros", &Borde_Inf, 240, NULL);

	}
	void sauvolaSimple(const cv::Mat &src, cv::Mat &dst, //Posible upgrade para otra versión del software
		int kernelSize, double k, double r)
	{
		dst.create(src.size(), src.type());

		cv::Mat srcWithBorder;
		int borderSize = kernelSize / 2 + 1;
		// 画像端の処理を簡単にするため外枠を付ける
		cv::copyMakeBorder(src, srcWithBorder,
			borderSize, borderSize, borderSize, borderSize, cv::BORDER_REPLICATE);

		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				// (x,y)周囲の部分行列を取得
				cv::Mat kernel = srcWithBorder(cv::Rect(x, y, kernelSize, kernelSize));
				// 部分行列の平均・標準偏差を求め、式から閾値を求める
				cv::Scalar mean, stddev;
				cv::meanStdDev(kernel, mean, stddev);
				double threshold = mean[0] * (1 + k * (stddev[0] / r - 1));
				// dstに白黒を設定
				if (src.at<uchar>(y, x) < threshold)
					dst.at<uchar>(y, x) = 0;
				else
					dst.at<uchar>(y, x) = 255;
			}
		}
	}

	Mat agregarROI(Mat& frame)
	{
		Point poly_points1[1][3];//POLIGONO 1
		//NOTA: Definir coordenadas en sentido horario
		//Según sentido ubicación aristas del polígono
		poly_points1[0][0] = Point(Vertice1_x, Vertice1_y);
		poly_points1[0][1] = Point(Vertice2_x, Vertice2_y);
		poly_points1[0][2] = Point(Vertice3_x, Vertice3_y);
		const Point* ppt1[1] = { poly_points1[0] };
		int npt1[] = { 3 };

		Point poly_points2[1][4];//Borde Superior
		poly_points2[0][0] = Point(0, 0);
		poly_points2[0][1] = Point(0, Borde_Sup);
		poly_points2[0][2] = Point(320, Borde_Sup);
		poly_points2[0][3] = Point(320, 0);
		const Point* ppt2[1] = { poly_points2[0] };
		int npt2[] = { 4 };

		Point poly_points3[1][4];//Borde Izquierdo
		poly_points3[0][0] = Point(0, 0);
		poly_points3[0][1] = Point(0, 240);
		poly_points3[0][2] = Point(Borde_Izq, 240);
		poly_points3[0][3] = Point(Borde_Izq, 0);
		const Point* ppt3[1] = { poly_points3[0] };
		int npt3[] = { 4 };

		Point poly_points4[1][4];//Borde Inferior
		poly_points4[0][0] = Point(0, Borde_Inf);
		poly_points4[0][1] = Point(0, 240);
		poly_points4[0][2] = Point(320, 240);
		poly_points4[0][3] = Point(320, Borde_Inf);
		const Point* ppt4[1] = { poly_points4[0] };
		int npt4[] = { 4 };

		Point poly_points5[1][4];//Borde Derecho
		poly_points5[0][0] = Point(Borde_Der, 0);
		poly_points5[0][1] = Point(Borde_Der, 240);
		poly_points5[0][2] = Point(320, 240);
		poly_points5[0][3] = Point(320, 0);
		const Point* ppt5[1] = { poly_points5[0] };
		int npt5[] = { 4 };

		fillPoly(frame, ppt1, npt1, 1, Scalar(0, 0, 0), 8); //Dibuja triangulo negro N.1
		fillPoly(frame, ppt2, npt2, 1, Scalar(0, 0, 0), 8); //Dibuja poligono negro N.2
		fillPoly(frame, ppt3, npt3, 1, Scalar(0, 0, 0), 8); //Dibuja poligono negro N.3
		fillPoly(frame, ppt4, npt4, 1, Scalar(0, 0, 0), 8); //Dibuja poligono negro N.4
		fillPoly(frame, ppt5, npt5, 1, Scalar(0, 0, 0), 8); //Dibuja poligono negro N.5
		return frame;
	}

	Mat operacionCierre(const Mat& imgThresh){

		int erosion_elem = 0;
		int erosion_size = 1;
		int erosion_type;

		if (erosion_elem == 0){ erosion_type = MORPH_RECT; }
		else if (erosion_elem == 1){ erosion_type = MORPH_CROSS; }
		else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

		Mat elementErode = getStructuringElement(erosion_type,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
		erode(imgThresh, imgThresh, elementErode);
		erode(imgThresh, imgThresh, elementErode);

		int dilation_elem = 0;
		int dilation_size = 1;
		int dilation_type;

		if (dilation_elem == 0){ dilation_type = MORPH_RECT; }
		else if (dilation_elem == 1){ dilation_type = MORPH_CROSS; }
		else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

		Mat elementDilation = getStructuringElement(dilation_type,
			Size(2 * dilation_size + 1, 2 * dilation_size + 1),
			Point(dilation_size, dilation_size));

		dilate(imgThresh, imgThresh, elementDilation);
		dilate(imgThresh, imgThresh, elementDilation);
		GaussianBlur(imgThresh, imgThresh, Size(3, 3), 0, 0);

		return Mat();
	}

	void MyFilledCircle(Mat img, Point2f center)
	{
		int thickness = 1;
		int lineType = 8;
		int w = 600.0;

		circle(img,
			center,
			w / 32.0,
			Scalar(0, 0, 255),
			thickness,
			lineType);
	}

}


int main(int argc, char** argv){
	//int i = 0;

	Mat frame;
	frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	Mat imgGRAY = Mat(frame.size(), CV_8UC3);
	//DireccionMAC(uuid, mac_addr);

	cout << "Bienvenido a Ñawuy" << endl;
	Sleep(1000);
	cout << "Creando archivo de registro txt..." << endl;
	Sleep(1000);
	string BasePath = "C:\\Niawuy\\";
	LPCWSTR LPCWSTRBasePath = L"C:\\Niawuy\\";
	CreateDirectory(LPCWSTRBasePath, NULL); //Crea carpeta si no existe
	cout << "El programa comienza en breve..." << endl;
	Ventanas();
	Mat imageMenu;
	string MenuNum = "Menu";
	string CamaraNum = "Menu num";
	string CamaraUmbral = "Menu umbral";
	string ParametrosNum = "Parametros";

	//Carga y despliega la imagen del menu
	imageMenu = imread("ñawi-logo.png", CV_LOAD_IMAGE_COLOR);   
	namedWindow(MenuNum, WINDOW_AUTOSIZE);
	imshow(MenuNum, imageMenu);

	//Ventana Parámetros
	//setwindowsettings();
	//while (1) {

		//Espera alguna tecla para acceder a comandos
		//las teclas son q,c,u,ESC,f,p.
		//int key = waitKey(10);

		//if ((char)key == 113) //Si presiona tecla q "Cierra Programa y volver al Menú principal"
		//{
		//	break;
		//}
		/*if ((char)key == 99) //Si presiona tecla c "Abre Cámara/Cierra Cámara"
		{
			
			if (flipCamara == true){
				cout << "Cierra Camara N. " << NumeroCamara << endl;
			}
			else{
				cout << "Abre Camara N. " << NumeroCamara << endl;
			}
			flipCamara = !flipCamara;
		}*/

		//if ((char)key == 117) //Si presiona tecla u "Abre Cámara/Cierra Umbral"
		//{
	/*
			if (flipCamaraUmbral == true){
				cout << "Esconder Umbralado" << endl;
			}
			else{
				cout << "Mostrar Umbralado" << endl;
			}
			flipCamaraUmbral = !flipCamaraUmbral;
		}

		if ((char)key == 27) //Si presiona tecla Esc "Cierra Programa"
		{
			myfile.close();
			cvDestroyAllWindows();
			cout << "Cerrando RatonVision-v1.5..." << endl;
			Sleep(1000);
			cout << "Gracias por Ñawi" << endl;
			Sleep(800);
			cout << "No olvide visitar www.ñawi.com" << endl;
			Sleep(800);
			return 0;
		}
		if ((char)key == 112) //Si presiona tecla p "Despliega/esconde menú de parámetros"
		{
			if (flipParameters == 1){
				cout << "Mostrar Parametros camara N." << endl;
				namedWindow(ParametrosNum, CV_WINDOW_NORMAL);
				moveWindow(ParametrosNum, 0, 0);

				createTrackbar("AreaMin", ParametrosNum, &areaMenor, 4e3, NULL);
				createTrackbar("AreaMax", ParametrosNum, &areaMayor, 4e4, NULL);

				createTrackbar("Linea", ParametrosNum, &Linea_, 300, NULL);

				createTrackbar("Umbral", ParametrosNum, &Umbral_, 255, NULL);

				createTrackbar("Vertice1x", ParametrosNum, &Vertice1_x, 300, NULL);
				createTrackbar("Vertice1y", ParametrosNum, &Vertice1_y, 300, NULL);

				createTrackbar("Vertice2x", ParametrosNum, &Vertice2_x, 300, NULL);
				createTrackbar("Vertice2y", ParametrosNum, &Vertice2_y, 300, NULL);

				createTrackbar("Vertice3x", ParametrosNum, &Vertice3_x, 300, NULL);
				createTrackbar("Vertice3y", ParametrosNum, &Vertice3_y, 300, NULL);

				//Valores maximos de trackbars de bordes definidos según imagen a trabajar 320x240 pix
				createTrackbar("BordeIzq", ParametrosNum, &Borde_Izq, 320, NULL);
				createTrackbar("BordeDer", ParametrosNum, &Borde_Der, 320, NULL);
				createTrackbar("BordeSup", ParametrosNum, &Borde_Sup, 240, NULL);
				createTrackbar("BordeInf", ParametrosNum, &Borde_Inf, 240, NULL);

			}
			else
			{
				cout << "Cerrar Parametros camara N." << endl;
				destroyWindow(ParametrosNum);
			}
			flipParameters = !flipParameters;
		}
		if ((char)key == 102) //Si presiona tecla f "Despliega/esconde Directorio de registro"
		{
			if (flipFile == 1){
				cout << "Abrir carpeta de registro" << endl;
				ShellExecute(NULL, NULL, LPCWSTRBasePath, NULL, NULL, SW_SHOWNOACTIVATE);
				//ShellExecute(NULL, NULL, NULL, NULL, LPCWSTRBasePath, SW_SHOWNOACTIVATE);
			}
			else{
				cout << "Esconder carpeta de registro" << endl;
				ShellExecute(NULL, NULL, LPCWSTRBasePath, NULL, NULL, SW_MINIMIZE);
				//ShellExecute(NULL, NULL, NULL, NULL, LPCWSTRBasePath, SW_MINIMIZE);
			}
			flipFile = !flipFile;
		}

		//capture >> frame;
		//if (frame.empty())
		//	break;
		//if (!bSuccess) //if not success, break loop
		//{
		//	cout << "Cannot read the frame from video file" << endl;
		//}
		*/
		frame = frame.clone();

		//imshow("AntesDeEqualizar", frame);
		equalizarHistograma(frame);
		//graficarHistograma(frame);

		/*
		//Muestra información de las distintas coordenadas de color: HSV, GRAY y YUV
		//Descomente para mostrar en consola
		Mat imgHSV;
		cvtColor(frame, imgHSV, CV_BGR2HSV);
		cv::Scalar avgPixelIntensityHSV = cv::mean(imgHSV);
		cout << "Pixel intensity HSV = " << avgPixelIntensityHSV.val[0] << endl;
		cout << "Pixel intensity HSV = " << avgPixelIntensityHSV.val[1] << endl;
		cout << "Pixel intensity HSV = " << avgPixelIntensityHSV.val[2] << endl;
		cout << "Pixel intensity HSV = " << avgPixelIntensityHSV.val[3] << endl;
		cout << "   " << endl;

		if (avgPixelIntensityHSV.val[0] > 111){
			cout << "Modo noche " << endl;
			Umbral_ = 140;}
			//O quizas usar setTrackPos
		
		Mat imgGRIS;
		cvtColor(frame, imgGRIS, CV_BGR2GRAY);
		cv::Scalar avgPixelIntensityGRAY = cv::mean(imgGRIS);
		cout << "Pixel intensity GRAY = " << avgPixelIntensityGRAY.val[0] << endl;
		cout << "Pixel intensity GRAY = " << avgPixelIntensityGRAY.val[1] << endl;
		cout << "Pixel intensity GRAY = " << avgPixelIntensityGRAY.val[2] << endl;
		cout << "Pixel intensity GRAY = " << avgPixelIntensityGRAY.val[3] << endl;
		cout << "   " << endl;
		Mat imgYUV;
		cvtColor(frame, imgYUV, CV_BGR2YCrCb);
		cv::Scalar avgPixelIntensityYUV = cv::mean(imgYUV);
		cout << "Pixel intensity YUV = " << avgPixelIntensityYUV.val[0] << endl;
		cout << "Pixel intensity YUV = " << avgPixelIntensityYUV.val[1] << endl;
		cout << "Pixel intensity YUV = " << avgPixelIntensityYUV.val[2] << endl;
		cout << "Pixel intensity YUV = " << avgPixelIntensityYUV.val[3] << endl;
		cout << "   " << endl;
		*/

		//agregarROI(frame);
		GaussianBlur(frame, frame, Size(3, 3), 0, 0);

		//Convierte a escala de grises
		Mat imgGrayScale = Mat(frame.size(), CV_8UC1);
		cvtColor(frame, imgGRAY, CV_BGR2GRAY);
		//namedWindow("Gray");
		//imshow("Gray", imgGRAY);

		/*Se utilizaron distintas tecnicas de umbralado o thresholding, entre ellas sauvola que es para estabilizar la
		luminosidad heterogenea del fondo de la imagen.
		*/

		threshold(imgGRAY, imgGRAY, 200, 255, 1);
		//threshold(imgGRAY, imgGRAY, Umbral_, 255, CV_THRESH_BINARY_INV);
		//sauvolaSimple(imgGRAY, imgGRAY, 17, 0.15, 50);
		//adaptiveThreshold(imgGRAY, imgGRAY, Umbral_, 1, 0, Neighborhood, Minus);
		
				
		/*if (flipCamaraUmbral == 1){ //Muestra/esconde umbralado
			
			namedWindow("Umbral");
			moveWindow("Umbral", 330, 460);
			imshow("Umbral", imgGRAY);
		}
		else{

			destroyWindow("Umbral");
		}
		*/



		operacionCierre(imgGRAY);// Hace erosion y dilatacion
		findContours(imgGRAY, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE); //Busca contornos

		//Resetea variables a, largest_area y largest_contour_index
		//Para redefinirlos según nuevo frame y nuevos contornos
		double a = 0;
		largest_area = 0;
		largest_contour_index = 0;
		
		for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			if (a>largest_area && a<areaMayor){
				largest_area = a;
				largest_contour_index = i;                //Store the index of largest contour
				bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
				
			}
			}

		//cout << "largest_contour_index =" << largest_contour_index << endl;
		//std::cout << typeid(largest_contour_index).name() << '\n';

		Moments oMoments = moments(contours[largest_contour_index]);//Calcula moment del blob mayor
		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;
		int deltaX = abs(iLastX - posX);
		int deltaY = abs(iLastY - posY);
		//cout << "deltax = " << deltaX << endl;
		//cout << "deltay = " << deltaY << endl;

		//Guarda datos de posicion en distintos formatos (Point y Char)
		Point xy = Point(posX, posY);
//		sprintf(centro, " %d %d ", posX, posY);
//		sprintf(centrotxt, " %d %d %d ", posX, posY, distanciareco);

		double area = largest_area;//Calcula area ratón

			//if (area < areaMenor || areaMayor < area) continue;//Filtra areas muy pequeñas y muy grandes
			if (area < areaMenor || areaMayor < area) {


				putText(frame, CamaraNum, Point(20, 30), FONT_HERSHEY_SIMPLEX, 0.6, (0, 242, 255), 2);
//				sprintf(distancia, "Distancia=%d", distanciareco);
				putText(frame, "Reajuste parametros", Point(180, 200), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));

				line(frame, cvPoint(0, Linea_), cvPoint(320, Linea_), cvScalar(0, 143, 255), 1.5); //---linea de parado/caminando
				putText(frame, "Reajuste parametros", cvPoint(30, 200), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));

				myfile << currentDateTime() << " NULL " << "NULL " << "NULL " << "NULL " << "NULL " << Umbral_ << " " << "\n";
				//void displayOverlay(const string& THRESH, const string& text, int delayms = 1000);

				if (flipCamara == true){
					namedWindow(CamaraNum);
					imshow(CamaraNum, frame);//Muestra el resultado final
				}
				else{
					destroyWindow(CamaraNum);
				}
				
			
			}//Filtra areas muy pequeñas y muy grandes
			else{

				putText(frame, centro, xy, FONT_HERSHEY_SIMPLEX, 0.5, (0, 143, 255));
				drawContours(frame, contours, largest_contour_index, CV_RGB(255, 0, 0), 2, 8, hierarchy, 0);

				//if (deltaX >= 9 && deltaY >= 5)
				//Registra solo cambios considerables de movimiento
				if (deltaX >= 7 || deltaY >= 5)
				{
					distanciareco += sqrt(((iLastX - posX)*(iLastX - posX)) + ((iLastY - posY)*(iLastY - posY)));

				}
				if (deltaX > 20 && deltaY > 15)
				{
					//cout << "Se agitó al ratón!!!" << endl;
				}
				iLastX = posX;
				iLastY = posY;

				Point cvpoint = Point(posX, posY);
				putText(frame, CamaraNum, Point(20, 30), FONT_HERSHEY_SIMPLEX, 0.6, (0, 242, 255), 2);
//				sprintf(distancia, "Distancia=%d", distanciareco);
//				sprintf(AreaChar, "Area=%d", area);
				putText(frame, distancia, Point(180, 200), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));

				//putText(frame, centro, cvpoint, FONT_HERSHEY_SIMPLEX, 0.5, (0, 143, 255));
				line(frame, cvPoint(0, Linea_), cvPoint(320, Linea_), cvScalar(0, 143, 255), 1.5); //---linea de parado/caminando

				if (posY < 105)
				{
					putText(frame, "parado", cvPoint(30, 200), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));
					estado = 1;
				}
				else
				{
					putText(frame, "NO parado", cvPoint(30, 200), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));
					estado = 0;
				}

				putText(frame, AreaChar, cvPoint(30, 220), FONT_HERSHEY_SIMPLEX, 0.5, (0, 142, 255));

				//Dibuja target sobre raton
				MyFilledCircle(frame, xy);
				line(frame, cvPoint(posX - 30, posY), cvPoint(posX + 30, posY), cvScalar(0, 255, 103), 1.5);
				line(frame, cvPoint(posX, posY - 30), cvPoint(posX, posY + 30), cvScalar(0, 255, 103), 1.5);
				//rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

				namedWindow(CamaraNum);
					imshow(CamaraNum, frame);//Muestra el resultado final

				/*if (flipCamara == true){
					namedWindow(CamaraNum);
					imshow(CamaraNum, frame);//Muestra el resultado final
				}
				else{
					destroyWindow(CamaraNum);
				}*/
				cout << "AQUI" << endl;
				myfile << centrotxt << estado << "\n";
				void displayOverlay(const string& THRESH, const string& text, int delayms = 1000);
			}

	//myfile.close();
	//cvDestroyAllWindows();
	cout << "Volviendo al Menu Principal" << endl;
	Sleep(800);
	cout << " " << endl;
	cout << " " << endl;
	//goto MENU;
	waitKey(0);                                          // Wait for a keystroke in the window
	return 0;
}