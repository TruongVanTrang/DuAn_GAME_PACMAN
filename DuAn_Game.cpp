#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

// Dinh nghia ma mau de su dung
#define MAU_DEN 0
#define MAU_XANH_DUONG 1
#define MAU_XANH_LA 2
#define MAU_XANH_NGOC 3
#define MAU_CYAN 3
#define MAU_DO 4
#define MAU_PINK 5
#define MAU_CAM 12
#define MAU_VANG 14
#define MAU_TRANG 15

int pacX = 350;
int pacY = 450;
int huongPac = 0;

// Ve Pac-Man ha mieng theo huong
void drawPacman(int x, int y, int huong) {
    setcolor(MAU_VANG);
    setfillstyle(SOLID_FILL, MAU_VANG);
    
    int stangle, endangle;
    if (huong == 0) { stangle = 45; endangle = 315; }   
    else if (huong == 1) { stangle = 315; endangle = 225; } 
    else if (huong == 2) { stangle = 225; endangle = 135; } 
    else if (huong == 3) { stangle = 135; endangle = 45; }  

    pieslice(x, y, stangle, endangle, 20);
}

// HAM VE CAC VAT PHAM PHU TRO
// Tham so 'loai': 1 = Giay, 2 = Thuc an lon, 3 = Hoa ma
void vePhuTro(int x, int y, int loai) {
    int banKinhLon = 18;

    if (loai == 1) { 
        // 1. PHU TRO GIAY (TANG TOC)
        setcolor(MAU_XANH_LA);
        circle(x, y, banKinhLon);
        
        setcolor(MAU_XANH_LA);
        settextstyle(2, 0, 4); 
        outtextxy(x - 14, y - 7, (char*)"GIAY"); 
    }
    else if (loai == 2) { 
        // 2. PHU TRO THUC AN LON
        setcolor(MAU_VANG); 
        circle(x, y, banKinhLon);
        
        setfillstyle(SOLID_FILL, MAU_VANG);
        fillellipse(x, y, 7, 7); 
    }
    else if (loai == 3) { 
        // 3. PHU TRO HOA MA
        setcolor(MAU_CYAN);
        circle(x, y, banKinhLon);
        
        // Ve mot con ma mini ben trong
        int my = y - 2; 
        setcolor(MAU_TRANG); 
        setfillstyle(SOLID_FILL, MAU_TRANG);
        
        pieslice(x, my, 0, 180, 8);     
        bar(x - 8, my, x + 8, my + 8);   
        
        setcolor(MAU_DEN);
        setfillstyle(SOLID_FILL, MAU_DEN);
        fillellipse(x - 3, my + 2, 2, 2); 
        fillellipse(x + 3, my + 2, 2, 2); 
    }
}

void drawFood(int x, int y) {
    setcolor(MAU_TRANG);
    setfillstyle(SOLID_FILL, MAU_TRANG);
    fillellipse(x, y, 3, 3);
}

// HAM RAI THUC AN VA VAT PHAM PHU TRO
void drawAllFood() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;

    // Rai thuc an theo hanh lang ngang tren
    for (int x = -420; x <= 420; x += 60) {
        if (x != 0) {
            drawFood(tamX + x, tamY - 300); 
        }
    }

    // Rai thuc an hanh lang duoi
    for (int x = -420; x <= 420; x += 60) {
        if (x != -420 && x != 420) {
            drawFood(tamX + x, tamY + 300); 
        }
    }

	// Rai thuc an hanh lang giua
    for (int x = -420; x <= 420; x += 60) {
        bool lotVaoChuongMa = (x >= -90 && x <= 90);
        bool lotVaoKhoiTrai = (x >= -270 && x <= -150);
        bool lotVaoKhoiPhai = (x >= 150 && x <= 270);

        if (!lotVaoChuongMa && !lotVaoKhoiTrai && !lotVaoKhoiPhai) {
            drawFood(tamX + x, tamY); 
        }
    }

	// Rai thuc an theo chieu doc
    for (int y = -300; y <= 300; y += 60) {
        if (y != -300 && y != 300) {
            drawFood(tamX - 420, tamY + y); 
            drawFood(tamX + 420, tamY + y); 
        }
        
        if (y <= -90 || y >= 90) {
            drawFood(tamX - 60, tamY + y); 
            drawFood(tamX + 60, tamY + y); 
        }
    }

    // Ve 3 vat pham phu tro vao cac vi tri da bo trong
    // Giay tang toc
    vePhuTro(tamX - 420, tamY + 300, 1); 
	// Thuc an lon
    vePhuTro(tamX + 420, tamY + 300, 2); 
    // Hoa ma o giua hanh lang tren
    vePhuTro(tamX, tamY - 300, 3); 
}

void veKhoi(int tamX, int tamY, int x1, int y1, int x2, int y2) {
    setfillstyle(SOLID_FILL, MAU_DEN); 
    bar(tamX + x1, tamY + y1, tamX + x2, tamY + y2);
    setcolor(MAU_XANH_NGOC);
    rectangle(tamX + x1, tamY + y1, tamX + x2, tamY + y2);
}

// HAM VE MAP ME CUNG PACMAN
void drawMaze() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;
    
    // Khung vien ngoai
    setcolor(MAU_XANH_NGOC);
    rectangle(tamX - 450, tamY - 330, tamX + 450, tamY + 330);
    rectangle(tamX - 455, tamY - 335, tamX + 455, tamY + 335);

   // Khu vuc chuong ma o giua
    veKhoi(tamX, tamY, -90, -30, 90, 30);
    setcolor(MAU_VANG); 
    line(tamX - 40, tamY - 30, tamX + 40, tamY - 30);

    // Cac khoi chuong ngai vat
    veKhoi(tamX, tamY, -30, -270, 30, -150);     
    veKhoi(tamX, tamY, -390, -270, -270, -210);  // Goc tren trai 1
    veKhoi(tamX, tamY, 270, -270, 390, -210);    // Goc tren phai 1
    veKhoi(tamX, tamY, -210, -270, -90, -150);   // Goc tren trai 2
    veKhoi(tamX, tamY, 90, -270, 210, -150);     // Goc tren phai 2
    veKhoi(tamX, tamY, -390, -150, -270, -90);   // Canh trai tren
    veKhoi(tamX, tamY, 270, -150, 390, -90);     // Canh phai tren

    veKhoi(tamX, tamY, -390, -30, -330, 150);    // Tru doc trai
    veKhoi(tamX, tamY, 330, -30, 390, 150);      // Tru doc phai
    veKhoi(tamX, tamY, -270, -30, -150, 30);     // Chan ngang trai
    veKhoi(tamX, tamY, 150, -30, 270, 30);       // Chan ngang phai

    veKhoi(tamX, tamY, -30, 90, 30, 270);        // Tru doc giua duoi
    veKhoi(tamX, tamY, -270, 90, -90, 150);      // Goc duoi trai 1
    veKhoi(tamX, tamY, 90, 90, 270, 150);        // Goc duoi phai 1
    veKhoi(tamX, tamY, -390, 210, -90, 270);     // Goc duoi trai 2
    veKhoi(tamX, tamY, 90, 210, 390, 270);       // Goc duoi phai 2 
}

// Ham ve con ma
void drawGhost(int x, int y, int color) {
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    pieslice(x, y, 0, 180, 15);
    bar(x - 15, y, x + 15, y + 20);
    
    setcolor(MAU_TRANG); setfillstyle(SOLID_FILL, MAU_TRANG);
    fillellipse(x - 6, y + 5, 4, 5); 
    fillellipse(x + 6, y + 5, 4, 5); 
    
    setcolor(MAU_XANH_DUONG); setfillstyle(SOLID_FILL, MAU_XANH_DUONG);
    fillellipse(x - 8, y + 5, 2, 2); 
    fillellipse(x + 4, y + 5, 2, 2); 
}

// HAM VE MENU
void drawMenu() {
    cleardevice();
    setbkcolor(MAU_DEN);

    // Lay toa do tam man hinh
    int tamX = getmaxx() / 2;
    int tamY = getmaxy() / 2;


    int khungTrai = tamX - 350;
    int khungPhai = tamX + 350;
    int khungTren = tamY - 250;
    int khungDuoi = tamY + 250;

    setcolor(MAU_XANH_DUONG);
    rectangle(khungTrai, khungTren, khungPhai, khungDuoi); 
    rectangle(khungTrai + 10, khungTren + 10, khungPhai - 10, khungDuoi - 10); 

    setcolor(MAU_VANG);
    settextstyle(1, 0, 7); 
    char title[] = "PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, khungTren + 50, title);

    settextstyle(3, 0, 5); 
    setcolor(MAU_TRANG);
    char m1[] = "1. BAT DAU CHOI";
    char m2[] = "2. HUONG DAN";
    char m3[] = "3. THOAT GAME";

    int yMenu = tamY - 30; 
    outtextxy(tamX - textwidth(m1)/2, yMenu, m1);
    outtextxy(tamX - textwidth(m2)/2, yMenu + 80, m2);
    outtextxy(tamX - textwidth(m3)/2, yMenu + 160, m3);

    setfillstyle(1, MAU_VANG);
    int pacSize = 30; 
    int pacY = yMenu + textheight(m1)/2; 
    
    pieslice(tamX - textwidth(m1)/2 - 70, pacY, 30, 330, pacSize); 
    pieslice(tamX + textwidth(m1)/2 + 70, pacY, 210, 150, pacSize); 
}

// HAM HIEN THI LUAT CHOI
void showRules() {
    cleardevice();
    setbkcolor(MAU_DEN);

    int tamX = getmaxx() / 2;
    int tamY = getmaxy() / 2;

    int khungTrai = tamX - 500;
    int khungPhai = tamX + 500;
    int khungTren = tamY - 250;
    int khungDuoi = tamY + 250;

    setcolor(MAU_TRANG);
    rectangle(khungTrai, khungTren, khungPhai, khungDuoi);
    rectangle(khungTrai + 10, khungTren + 10, khungPhai - 10, khungDuoi - 10);

    setcolor(MAU_XANH_NGOC);
    settextstyle(1, 0, 5); 
    char title[] = "LUAT CHOI PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, khungTren + 40, title);

    settextstyle(3, 0, 3); 
    int xText = khungTrai + 50; 
    int yText = khungTren + 130;
    int gianDong = 40; 

    setcolor(MAU_TRANG);
    outtextxy(xText, yText, (char*)"1. Dung mui ten de di chuyen Pac-Man.");
    outtextxy(xText, yText + gianDong*1, (char*)"2. Pac-Man se di chuyen lien tuc va chi dung lai khi gap vat can.");
    outtextxy(xText, yText + gianDong*2, (char*)"3. Nuot nhung diem thuc an tren duong di - ban se duoc cong diem.");
    outtextxy(xText, yText + gianDong*3, (char*)"4. Hay tranh nhung con ma - chung se lam ban mat mang khi cham vao (toi da 3 lan).");
    outtextxy(xText, yText + gianDong*4, (char*)"5. Ngoai diem thuc an co ban se co nhung vat pham phu tro:");
	
	// Vat pham phu tro
    int xSubText = xText + 50;
    setcolor(MAU_VANG); 
    outtextxy(xSubText, yText + gianDong*5, (char*)"- Giay: Tang toc do di chuyen trong 5s.");
    outtextxy(xSubText, yText + gianDong*6, (char*)"- Thuc an lon: Nhieu diem to hon, nhieu diem hon trong 10s.");
    outtextxy(xSubText, yText + gianDong*7, (char*)"- Hoa ma: Bien thanh ma den, co the giet nguoc lai ma khac trong 3s.");

    setcolor(MAU_XANH_NGOC);
    settextstyle(3, 0, 3);
    char footer[] = "An phim bat ky de quay lai Menu...";
    outtextxy(tamX - textwidth(footer)/2, khungDuoi - 50, footer);

    getch();
}

// --- HAM VE GIAO DIEN HUONG DAN
void drawUIBenTrai() {
    int lX = 40; 
    int topY = 100; 

    // --- 1. KHU VUC HUONG DAN DIEU KHIEN ---
    setcolor(MAU_VANG);
    settextstyle(1, 0, 2); 
    outtextxy(lX, topY, (char*)"1. DIEU KHIEN:");

    setcolor(MAU_TRANG);
    settextstyle(2, 0, 4); 
    
    rectangle(lX + 80, topY + 40, lX + 130, topY + 90);
    outtextxy(lX + 92, topY + 58, (char*)"LEN");
    
    rectangle(lX + 20, topY + 100, lX + 70, topY + 150);
    outtextxy(lX + 26, topY + 118, (char*)"TRAI");
    
    rectangle(lX + 80, topY + 100, lX + 130, topY + 150);
    outtextxy(lX + 84, topY + 118, (char*)"XUONG");
    
    rectangle(lX + 140, topY + 100, lX + 190, topY + 150);
    outtextxy(lX + 147, topY + 118, (char*)"PHAI");

	// --- 2. Khu vuc vat pham
    setcolor(MAU_VANG);
    settextstyle(1, 0, 2);
    outtextxy(lX, topY + 180, (char*)"2. VAT PHAM:");

    vePhuTro(lX + 30, topY + 250, 1); 
    setcolor(MAU_TRANG); 
    settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 238, (char*)"- Giay: Tang toc do (5s)");

    vePhuTro(lX + 30, topY + 320, 2);
    setcolor(MAU_TRANG);
    settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 308, (char*)"- Thuc an lon: +Nhieu diem (10s)");

    vePhuTro(lX + 30, topY + 390, 3);
    setcolor(MAU_TRANG);
    settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 378, (char*)"- Hoa ma: An lai duoc ma (3s)");
}

// HAM BAT DAU TRO CHOI
void startGame() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;
    
    pacX = tamX; 
    pacY = tamY + 300;
    huongPac = 0;
    int ch;
    int trangVao = 0; 

    int diemSo = 0;

    time_t thoiGianBatDau = time(NULL); 

    while (true) {
        setactivepage(trangVao); 
        
        cleardevice();
        drawMaze();
        drawAllFood();
        drawUIBenTrai();

        drawGhost(tamX, tamY, MAU_DO);               
        drawGhost(tamX - 420, tamY - 300, MAU_PINK); 
        drawGhost(tamX + 420, tamY - 300, MAU_CYAN); 

        drawPacman(pacX, pacY, huongPac);

        setcolor(MAU_VANG);
        settextstyle(0, 0, 1); 
        outtextxy(50, 20, (char*)"An ESC de quay ve Menu");

        char strDiem[50];
        sprintf(strDiem, (char*)"Diem so: %d", diemSo);
        setcolor(MAU_VANG); 
        settextstyle(1, 0, 3); 
        
        outtextxy(50, getmaxy() - 100, strDiem);

        // Bat dau dem thoi gian
        time_t thoiGianHienTai = time(NULL);
        int daTroiQua = difftime(thoiGianHienTai, thoiGianBatDau);
        int thoiGianConLai = 20 - daTroiQua; 
        
        char strThoiGian[50];
        sprintf(strThoiGian, (char*)"Thoi gian: %d s", thoiGianConLai);
        setcolor(MAU_TRANG);
        settextstyle(1, 0, 3); 
        outtextxy(getmaxx() - 250, 20, strThoiGian);

        //Kiem tra thua game
        if (thoiGianConLai <= 0) {
            setcolor(MAU_VANG);
            settextstyle(1, 0, 8); 
            char msgThua[] = "THAT BAI";
            outtextxy(tamX - textwidth(msgThua)/2, tamY - textheight(msgThua)/2 - 50, msgThua);

            setcolor(MAU_TRANG);
            settextstyle(3, 0, 4);
            char msgThoat[] = "Ban thua roi - An phim bat ky de quay lai...";
            outtextxy(tamX - textwidth(msgThoat)/2, tamY + 50, msgThoat);

            setvisualpage(trangVao); 
            getch(); 
            break;   
        }

        setvisualpage(trangVao); 
        trangVao = 1 - trangVao; 

        if (kbhit()) {
            ch = getch();
            if (ch == 27) break; 

            if (ch == 0 || ch == 224) {
                ch = getch();
                if (ch == 72) { huongPac = 3; pacY -= 15; } 
                if (ch == 80) { huongPac = 1; pacY += 15; } 
                if (ch == 75) { huongPac = 2; pacX -= 15; } 
                if (ch == 77) { huongPac = 0; pacX += 15; } 
            }
        }
        delay(40); 
    }
    
    setactivepage(0);
    setvisualpage(0);
}

int main() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    initwindow(screenWidth, screenHeight, "Pac-Man Game", -3, -3);

    char key;
    while (true) {
        drawMenu();
        key = getch();   

        if (key == '1') {
            startGame();
        }
        else if (key == '2') {
            showRules();
        }
        else if (key == '3') {
            closegraph();
            return 0;
        }
    }
}
