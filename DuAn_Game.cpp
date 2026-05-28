#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>

// --- DINH NGHIA MA MAU ---
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

bool thucAn[15][11];
bool giayTonTai = true;
bool thucAnLonTonTai = true;
bool hoaMaTonTai = true;

struct Ghost {
    int x, y;
    int huong; 
    int mau;
    bool isActive; 
    int releaseTime; 
    int startX, startY; 
};
Ghost ma[3];

// =================================================================
// PHAN 1: CAC THUAT TOAN DO HOA CO BAN (CHUAN CHUONG 2)
// =================================================================

// 1. Thuat toan Bresenham ve doan thang
void veDuongThang(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int stepX = (x1 < x2) ? 1 : -1, stepY = (y1 < y2) ? 1 : -1;
    int p = (dx > dy ? dx : -dy) / 2, e2;

    while (true) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = p;
        if (e2 > -dx) { p -= dy; x1 += stepX; }
        if (e2 < dy) { p += dx; y1 += stepY; }
    }
}

// 2. Ham phu tro ve Hinh Chu Nhat
void veHinhChuNhat(int left, int top, int right, int bottom, int color) {
    veDuongThang(left, top, right, top, color);       
    veDuongThang(right, top, right, bottom, color);   
    veDuongThang(right, bottom, left, bottom, color); 
    veDuongThang(left, bottom, left, top, color);     
}

// 3. Thuat toan Midpoint ve Duong tron
void veDuongTron(int xc, int yc, int r, int color) {
    int x = 0, y = r, p = 1 - r;
    while (x <= y) {
        putpixel(xc + x, yc + y, color); putpixel(xc - x, yc + y, color);
        putpixel(xc + x, yc - y, color); putpixel(xc - x, yc - y, color);
        putpixel(xc + y, yc + x, color); putpixel(xc - y, yc + x, color);
        putpixel(xc + y, yc - x, color); putpixel(xc - y, yc - x, color);
        x++;
        if (p < 0) { p += 2 * x + 1; } else { y--; p += 2 * (x - y) + 1; }
    }
}

// 4. Thuat toan To mau De quy (Flood Fill)
void toMauDeQuy(int x, int y, int mauTo, int mauNenCu) {
    int mauHienTai = getpixel(x, y);
    if (mauHienTai == mauNenCu && mauHienTai != mauTo) {
        putpixel(x, y, mauTo);
        toMauDeQuy(x + 1, y, mauTo, mauNenCu); 
        toMauDeQuy(x - 1, y, mauTo, mauNenCu); 
        toMauDeQuy(x, y + 1, mauTo, mauNenCu); 
        toMauDeQuy(x, y - 1, mauTo, mauNenCu); 
    }
}

// =================================================================
// PHAN 2: THUAT TOAN FRACTAL (CHUAN CHUONG 3)
// =================================================================

// 5. Thuat toan ve Cay Fractal (De quy trang tri UI)
void veCayFractal(int x, int y, float goc, int chieuDai, int n, int color) {
    if (n == 0) return; // Dieu kien dung
    
    int x2 = x + (int)(chieuDai * cos(goc));
    int y2 = y - (int)(chieuDai * sin(goc)); // Y giam = huong len tren
    
    veDuongThang(x, y, x2, y2, color);
    
    // De quy re 2 nhanh tich cuc (Goc toa ra 0.5 radian)
    veCayFractal(x2, y2, goc - 0.5, chieuDai * 0.75, n - 1, color);
    veCayFractal(x2, y2, goc + 0.5, chieuDai * 0.75, n - 1, color);
}

// 6. Thuat toan ve Duong cong Koch (Ho tro tao Bong Tuyet)
void veDuongKoch(int n, float x1, float y1, float x2, float y2, int color) {
    if (n == 0) {
        veDuongThang((int)x1, (int)y1, (int)x2, (int)y2, color);
    } else {
        float dx = (x2 - x1) / 3.0;
        float dy = (y2 - y1) / 3.0;
        
        float xA = x1 + dx; float yA = y1 + dy;
        float xC = x1 + 2 * dx; float yC = y1 + 2 * dy;
        
        // Tinh toa do dinh B (Phep xoay 60 do)
        float xB = xA + dx * 0.5 + dy * 0.866025; 
        float yB = yA - dx * 0.866025 + dy * 0.5;
        
        veDuongKoch(n - 1, x1, y1, xA, yA, color);
        veDuongKoch(n - 1, xA, yA, xB, yB, color);
        veDuongKoch(n - 1, xB, yB, xC, yC, color);
        veDuongKoch(n - 1, xC, yC, x2, y2, color);
    }
}

// 7. Thuat toan tao Bong Tuyet Koch (Thuc an lon)
void veBongTuyetKoch(int x, int y, int size, int n, int color) {
    float h = size * 0.866025; // Chieu cao tam giac deu
    float x1 = x, y1 = y - size;
    float x2 = x - h, y2 = y + size / 2.0;
    float x3 = x + h, y3 = y + size / 2.0;

    // Ve 3 canh Koch hep lai thanh vong tron
    veDuongKoch(n, x2, y2, x1, y1, color);
    veDuongKoch(n, x1, y1, x3, y3, color);
    veDuongKoch(n, x3, y3, x2, y2, color);
}

// =================================================================
// PHAN 3: VE NHAN VAT VA VAT PHAM TRONG GAME
// =================================================================

void drawPacman(int x, int y, int huong) {
    int r = 20;
    for(int i = y - 21; i <= y + 21; i++) veDuongThang(x - 21, i, x + 21, i, MAU_DEN); // Quet nen

    veDuongTron(x, y, r, MAU_VANG);

    int mx1, my1, mx2, my2, xLung, yLung;       
    if (huong == 0) { mx1 = x+16; my1 = y-12; mx2 = x+16; my2 = y+12; xLung = x-10; yLung = y; } 
    else if (huong == 2) { mx1 = x-16; my1 = y-12; mx2 = x-16; my2 = y+12; xLung = x+10; yLung = y; } 
    else if (huong == 1) { mx1 = x-12; my1 = y+16; mx2 = x+12; my2 = y+16; xLung = x; yLung = y-10; } 
    else if (huong == 3) { mx1 = x-12; my1 = y-16; mx2 = x+12; my2 = y-16; xLung = x; yLung = y+10; }

    veDuongThang(x, y, mx1, my1, MAU_VANG); veDuongThang(x, y, mx2, my2, MAU_VANG);
    toMauDeQuy(xLung, yLung, MAU_VANG, MAU_DEN);

    if (huong == 0) for(int i=x+16; i<=x+21; i++) veDuongThang(i, y-12, i, y+12, MAU_DEN);
    else if (huong == 2) for(int i=x-21; i<=x-16; i++) veDuongThang(i, y-12, i, y+12, MAU_DEN);
    else if (huong == 1) for(int i=y+16; i<=y+21; i++) veDuongThang(x-12, i, x+12, i, MAU_DEN);
    else if (huong == 3) for(int i=y-21; i<=y-16; i++) veDuongThang(x-12, i, x+12, i, MAU_DEN);
}

void drawGhost(int x, int y, int color) {
    for(int i = y - 16; i <= y + 21; i++) veDuongThang(x - 16, i, x + 16, i, MAU_DEN); // Quet nen
    veDuongTron(x, y, 15, color); 
    veDuongThang(x - 15, y, x - 15, y + 20, color); veDuongThang(x + 15, y, x + 15, y + 20, color);      
    veDuongThang(x - 15, y + 20, x + 15, y + 20, color); 

    toMauDeQuy(x, y, color, MAU_DEN); toMauDeQuy(x, y + 18, color, MAU_DEN); 

    veDuongTron(x - 6, y + 3, 4, MAU_TRANG); toMauDeQuy(x - 6, y + 3, MAU_TRANG, color); 
    veDuongTron(x + 6, y + 3, 4, MAU_TRANG); toMauDeQuy(x + 6, y + 3, MAU_TRANG, color);

    veDuongTron(x - 8, y + 3, 1, MAU_XANH_DUONG); putpixel(x - 8, y + 3, MAU_XANH_DUONG);
    veDuongTron(x + 4, y + 3, 1, MAU_XANH_DUONG); putpixel(x + 4, y + 3, MAU_XANH_DUONG);
}

void drawFood(int x, int y) {
    veDuongTron(x, y, 3, MAU_TRANG);
    toMauDeQuy(x, y, MAU_TRANG, MAU_DEN); 
}

void vePhuTro(int x, int y, int loai) {
    int banKinhLon = 18;
    if (loai == 1) { 
        veDuongTron(x, y, banKinhLon, MAU_XANH_LA);
        setcolor(MAU_XANH_LA); settextstyle(2, 0, 4); outtextxy(x - 14, y - 7, (char*)"GIAY"); 
    } 
    else if (loai == 2) { 
        // CHUONG 3: UNG DUNG BONG TUYET KOCH VAO THUC AN LON
        veBongTuyetKoch(x, y, 14, 3, MAU_VANG); // Ve vong ngoai bang Fractal
        veDuongTron(x, y, 4, MAU_VANG);         // Loi sang o giua
        toMauDeQuy(x, y, MAU_VANG, MAU_DEN); 
    }
    else if (loai == 3) { 
        veDuongTron(x, y, banKinhLon, MAU_CYAN); 
        int my = y - 2; 
        veDuongTron(x, my, 8, MAU_TRANG); veDuongThang(x - 8, my, x - 8, my + 10, MAU_TRANG);    
        veDuongThang(x + 8, my, x + 8, my + 10, MAU_TRANG); veDuongThang(x - 8, my + 10, x + 8, my + 10, MAU_TRANG);
        toMauDeQuy(x, my, MAU_TRANG, MAU_DEN); toMauDeQuy(x, my + 9, MAU_TRANG, MAU_DEN); 
        veDuongTron(x - 3, my + 2, 1, MAU_DEN); putpixel(x - 3, my + 2, MAU_DEN);
        veDuongTron(x + 3, my + 2, 1, MAU_DEN); putpixel(x + 3, my + 2, MAU_DEN);
    }
}

// =================================================================
// PHAN 4: XAY DUNG BAN DO & QUET TOA DO
// =================================================================
bool isInside(int x, int y, int x1, int y1, int x2, int y2) { return (x >= x1 && x <= x2 && y >= y1 && y <= y2); }

bool laVatCan(int x, int y) {
    // 1. Khung ngoai
    if (x <= -450 || x >= 450 || y <= -330 || y >= 330) return true;

    // 2. Kiem tra 27 khoi trong drawMaze
    // Truc giua
    if (isInside(x, y, -90, -30, 90, 30)) return true;
    if (isInside(x, y, -150, -270, 150, -210)) return true;
    if (isInside(x, y, -30, -210, 30, -150)) return true;
    if (isInside(x, y, -150, 210, 150, 270)) return true;
    if (isInside(x, y, -30, 150, 30, 210)) return true;

    // Goc tren trai
    if (isInside(x, y, -390, -270, -330, -150)) return true;
    if (isInside(x, y, -330, -270, -210, -210)) return true;

    // Goc tren phai
    if (isInside(x, y, 330, -270, 390, -150)) return true;
    if (isInside(x, y, 210, -270, 330, -210)) return true;

    // Goc duoi trai
    if (isInside(x, y, -390, 210, -330, 270)) return true;
    if (isInside(x, y, -330, 210, -210, 270)) return true;

    // Goc duoi phai
    if (isInside(x, y, 330, 150, 390, 270)) return true;
    if (isInside(x, y, 210, 210, 330, 270)) return true;

    // Cac khoi doc hai ben ngoai cung
    if (isInside(x, y, -450, -90, -330, -30)) return true;
    if (isInside(x, y, -450, -30, -390, 30)) return true;
    if (isInside(x, y, 330, -90, 450, -30)) return true;
    if (isInside(x, y, 330, 30, 450, 90)) return true;

    // Cac khoi roi rac trai/phai
    if (isInside(x, y, -390, 90, -330, 150)) return true;
    if (isInside(x, y, -330, 30, -270, 90)) return true;
    if (isInside(x, y, -270, -150, -150, -90)) return true;
    if (isInside(x, y, 150, -150, 270, -90)) return true;
    if (isInside(x, y, -270, 90, -150, 150)) return true;
    if (isInside(x, y, 210, -90, 270, 150)) return true;
    if (isInside(x, y, -270, -30, -150, 30)) return true;
    if (isInside(x, y, 150, -30, 270, 30)) return true;

    // Cac khoi nho noi truc giua
    if (isInside(x, y, -90, -150, 90, -90)) return true;
    if (isInside(x, y, -90, 90, 150, 150)) return true;

    return false; 
}

void drawAllFood() {
    int tamX = getmaxx() / 2 + 150, tamY = getmaxy() / 2;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 11; j++) {
            if (thucAn[i][j]) {
                int x = -420 + i * 60;
                int y = -300 + j * 60;
                drawFood(tamX + x, tamY + y);
            }
        }
    }
    if (giayTonTai) vePhuTro(tamX - 420, tamY + 300, 1); 
    if (thucAnLonTonTai) vePhuTro(tamX + 420, tamY + 300, 2); 
    if (hoaMaTonTai) vePhuTro(tamX, tamY - 300, 3); 
}

void veKhoi(int tamX, int tamY, int x1, int y1, int x2, int y2) {
    for(int i = y1 + 1; i < y2; i++) veDuongThang(tamX + x1 + 1, tamY + i, tamX + x2 - 1, tamY + i, MAU_DEN);
    veHinhChuNhat(tamX + x1, tamY + y1, tamX + x2, tamY + y2, MAU_XANH_NGOC);
}

void drawMaze() {
    int tamX = getmaxx() / 2 + 150, tamY = getmaxy() / 2;
    veHinhChuNhat(tamX - 450, tamY - 330, tamX + 450, tamY + 330, MAU_XANH_NGOC);
    veHinhChuNhat(tamX - 455, tamY - 335, tamX + 455, tamY + 335, MAU_XANH_NGOC);

    veKhoi(tamX, tamY, -90, -30, 90, 30); veDuongThang(tamX - 40, tamY - 30, tamX + 40, tamY - 30, MAU_VANG); 
    
    veKhoi(tamX, tamY, -150, -270, 150, -210); veKhoi(tamX, tamY, -30, -210, 30, -150);   
    veDuongThang(tamX - 29, tamY - 210, tamX + 29, tamY - 210, MAU_DEN); 
    veKhoi(tamX, tamY, -150, 210, 150, 270); veKhoi(tamX, tamY, -30, 150, 30, 210);   
    veDuongThang(tamX - 29, tamY + 210, tamX + 29, tamY + 210, MAU_DEN);

    veKhoi(tamX, tamY, -390, -270, -330, -150); veKhoi(tamX, tamY, -330, -270, -210, -210); 
    veDuongThang(tamX - 330, tamY - 269, tamX - 330, tamY - 211, MAU_DEN);
    veKhoi(tamX, tamY, 330, -270, 390, -150); veKhoi(tamX, tamY, 210, -270, 330, -210); 
    veDuongThang(tamX + 330, tamY - 269, tamX + 330, tamY - 211, MAU_DEN);
    
	veKhoi(tamX, tamY, -390, 210, -330, 270); veKhoi(tamX, tamY, -330, 210, -210, 270); 
    veDuongThang(tamX - 330, tamY + 211, tamX - 330, tamY + 269, MAU_DEN);
    veKhoi(tamX, tamY, 330, 150, 390, 270); veKhoi(tamX, tamY, 210, 210, 330, 270); 
    veDuongThang(tamX + 330, tamY + 211, tamX + 330, tamY + 269, MAU_DEN);

    ///
	veKhoi(tamX, tamY, -450, -90, -330, -30); veKhoi(tamX, tamY, -450, -30, -390, 30);
    veDuongThang(tamX - 449, tamY - 30, tamX - 391, tamY - 30, MAU_DEN); // Xoa vien chong len nhau

    veKhoi(tamX, tamY, 330, -90, 450, -30); veKhoi(tamX, tamY, 330, 30, 450, 90);     
	
	//
	 veKhoi(tamX, tamY, -390, 90, -330, 150); veKhoi(tamX, tamY, -330, 30, -270, 90);
 
    veKhoi(tamX, tamY, -270, -150, -150, -90); veKhoi(tamX, tamY, 150, -150, 270, -90);   
    ///---
    veKhoi(tamX, tamY, -270, 90, -150, 150); veKhoi(tamX, tamY, 210, -90, 270, 150);     
    veDuongThang(tamX + 211, tamY - 90, tamX + 269, tamY - 90, MAU_DEN); // Noi khoi doc phai

    veKhoi(tamX, tamY, -270, -30, -150, 30); veKhoi(tamX, tamY, 150, -30, 270, 30);   
    // Xoa vien cho phan giao nhau cua khoi ngang va khoi doc ben phai
    veDuongThang(tamX + 211, tamY - 30, tamX + 269, tamY - 30, MAU_DEN);
    veDuongThang(tamX + 211, tamY + 30, tamX + 269, tamY + 30, MAU_DEN);
    veDuongThang(tamX + 210, tamY - 29, tamX + 210, tamY + 29, MAU_DEN);
    veKhoi(tamX, tamY, -90, -150, 90, -90); veDuongThang(tamX - 29, tamY - 150, tamX + 29, tamY - 150, MAU_DEN); 
    veKhoi(tamX, tamY, -90, 90, 150, 150); veDuongThang(tamX - 29, tamY + 150, tamX + 29, tamY + 150, MAU_DEN); 
}

// =================================================================
// PHAN 5: GIAO DIEN & MENU (XU LY TRONG SUOT CHU)
// =================================================================

void drawUIBenTrai() {
    int lX = 40, topY = 100; 

    setcolor(MAU_VANG); settextstyle(1, 0, 2); outtextxy(lX, topY, (char*)"1. DIEU KHIEN:");
    setcolor(MAU_TRANG); settextstyle(2, 0, 4); 
    
    veHinhChuNhat(lX + 80, topY + 40, lX + 130, topY + 90, MAU_TRANG); outtextxy(lX + 92, topY + 58, (char*)"LEN");
    veHinhChuNhat(lX + 20, topY + 100, lX + 70, topY + 150, MAU_TRANG); outtextxy(lX + 26, topY + 118, (char*)"TRAI");
    veHinhChuNhat(lX + 80, topY + 100, lX + 130, topY + 150, MAU_TRANG); outtextxy(lX + 84, topY + 118, (char*)"XUONG");
    veHinhChuNhat(lX + 140, topY + 100, lX + 190, topY + 150, MAU_TRANG); outtextxy(lX + 147, topY + 118, (char*)"PHAI");

    setcolor(MAU_VANG); settextstyle(1, 0, 2); outtextxy(lX, topY + 180, (char*)"2. VAT PHAM:");
    vePhuTro(lX + 30, topY + 250, 1); setcolor(MAU_TRANG); settextstyle(3, 0, 2); outtextxy(lX + 70, topY + 238, (char*)"- Giay: Tang toc do (5s)");
    vePhuTro(lX + 30, topY + 320, 2); setcolor(MAU_TRANG); settextstyle(3, 0, 2); outtextxy(lX + 70, topY + 308, (char*)"- Thuc an lon: +Nhieu diem (10s)");
    vePhuTro(lX + 30, topY + 390, 3); setcolor(MAU_TRANG); settextstyle(3, 0, 2); outtextxy(lX + 70, topY + 378, (char*)"- Hoa ma: An lai duoc ma (3s)");

    // CHUONG 3: UNG DUNG CAY FRACTAL TRANG TRI NGOAI CANH
    setcolor(MAU_XANH_LA); settextstyle(3, 0, 2); 
    outtextxy(lX + 15, topY + 470, (char*)"[Rung Ma Thuat Fractal]");
    // Ve cay fractal (goc pi/2 tuc la 1.57 huong len), bac 6
    veCayFractal(lX + 120, topY + 620, 1.57, 45, 6, MAU_XANH_LA);
}

void drawMenu() {
    cleardevice(); setbkcolor(MAU_DEN); 
    int tamX = getmaxx() / 2, tamY = getmaxy() / 2;
    int kTrai = tamX - 350, kPhai = tamX + 350, kTren = tamY - 250, kDuoi = tamY + 250;

    veHinhChuNhat(kTrai, kTren, kPhai, kDuoi, MAU_XANH_DUONG); veHinhChuNhat(kTrai + 10, kTren + 10, kPhai - 10, kDuoi - 10, MAU_XANH_DUONG); 

    setcolor(MAU_VANG); settextstyle(1, 0, 7); char title[] = "PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, kTren + 40, title);

    settextstyle(3, 0, 5); setcolor(MAU_TRANG);
    char m1[] = "1. BAT DAU CHOI", m2[] = "2. HUONG DAN", m3[] = "3. THOAT GAME";

    int yMenu = tamY - 30; 
    outtextxy(tamX - textwidth(m1)/2, yMenu, m1); outtextxy(tamX - textwidth(m2)/2, yMenu + 70, m2); outtextxy(tamX - textwidth(m3)/2, yMenu + 140, m3);

    int pacSize = 30, pacYM = yMenu + textheight(m1)/2; 
    setfillstyle(SOLID_FILL, MAU_VANG);
    pieslice(tamX - textwidth(m1)/2 - 70, pacYM, 30, 330, pacSize); pieslice(tamX + textwidth(m1)/2 + 70, pacYM, 210, 150, pacSize); 
}

void showRules() {
    cleardevice(); setbkcolor(MAU_DEN); 
    int tamX = getmaxx() / 2, tamY = getmaxy() / 2;
    int kTrai = tamX - 500, kPhai = tamX + 500, kTren = tamY - 280, kDuoi = tamY + 280;

    veHinhChuNhat(kTrai, kTren, kPhai, kDuoi, MAU_TRANG); veHinhChuNhat(kTrai + 10, kTren + 10, kPhai - 10, kDuoi - 10, MAU_TRANG);

    setcolor(MAU_XANH_NGOC); settextstyle(1, 0, 5); char title[] = "LUAT CHOI PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, kTren + 30, title);

    settextstyle(3, 0, 3); int xT = kTrai + 40, yT = kTren + 110, gD = 45; 

    setcolor(MAU_TRANG);
    outtextxy(xT, yT, (char*)"1. Dung mui ten de di chuyen Pac-Man.");
    outtextxy(xT, yT + gD*1, (char*)"2. Pac-Man chi dung lai khi gap vat can.");
    outtextxy(xT, yT + gD*2, (char*)"3. Nuot thuc an tren duong de cong diem.");
    outtextxy(xT, yT + gD*3, (char*)"4. Tranh ma de khong bi mat mang.");
    outtextxy(xT, yT + gD*4, (char*)"5. Cac vat pham phu tro:");
    
    int xS = xT + 40; setcolor(MAU_VANG); 
    outtextxy(xS, yT + gD*5, (char*)"- Giay: Tang toc do di chuyen (5s).");
    outtextxy(xS, yT + gD*6, (char*)"- Thuc an lon: Diem to hon (10s).");
    outtextxy(xS, yT + gD*7, (char*)"- Hoa ma: Co the an lai duoc ma (3s).");

    setcolor(MAU_XANH_NGOC); settextstyle(3, 0, 3);
    char footer[] = "An phim bat ky de quay lai Menu...";
    outtextxy(tamX - textwidth(footer)/2, kDuoi - 45, footer);
    getch();
}

void hienThiGameOver(int diemSo) {
    setactivepage(0); setvisualpage(0);
    int tamX = getmaxx() / 2 + 150, tamY = getmaxy() / 2;
    setcolor(MAU_DO);
    settextstyle(4, 0, 8);
    char msg[] = "GAME OVER";
    outtextxy(tamX - textwidth(msg)/2, tamY - 50, msg);
    
    setcolor(MAU_VANG);
    settextstyle(1, 0, 4);
    char str[50];
    sprintf(str, "Diem so cua ban: %d", diemSo);
    outtextxy(tamX - textwidth(str)/2, tamY + 50, str);
    
    setcolor(MAU_TRANG);
    settextstyle(3, 0, 3);
    char footer[] = "An phim bat ky de quay lai Menu...";
    outtextxy(tamX - textwidth(footer)/2, tamY + 120, footer);
    getch();
}

// =================================================================
// PHAN 6: ENGINE GAME CHINH (DA THEM TRANG THAI "CHO" - READY STATE)
// =================================================================
void startGame() {
    int tamX = getmaxx() / 2 + 150, tamY = getmaxy() / 2;
    pacX = tamX; pacY = tamY + 300; 
    huongPac = 0; // Mac dinh quay mat sang phai
    
    // Khoi tao mang thuc an
    giayTonTai = true; thucAnLonTonTai = true; hoaMaTonTai = true;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 11; j++) {
            int x = -420 + i * 60;
            int y = -300 + j * 60;
            if (x == -420 && y == 300) thucAn[i][j] = false; 
            else if (x == 420 && y == 300) thucAn[i][j] = false;  
            else if (x == 0 && y == -300) thucAn[i][j] = false;   
            else if (!laVatCan(x, y)) thucAn[i][j] = true;
            else thucAn[i][j] = false;
        }
    }

    // Khoi tao Ma
    ma[0] = {tamX, tamY - 60, 0, MAU_DO, false, 2, tamX, tamY - 60}; 
    ma[1] = {tamX - 40, tamY, 0, MAU_PINK, false, 10, tamX - 40, tamY};
    ma[2] = {tamX + 40, tamY, 0, MAU_CYAN, false, 15, tamX + 40, tamY};

    int ch, trangVao = 0, diemSo = 0;
    bool daBatDau = false; 
    int tocDo = 20;
    int nextHuong = huongPac;
    clock_t startTime = 0;

    while (true) {
        setactivepage(trangVao); 
        cleardevice();
        
        drawMaze(); 
        drawAllFood(); 
        drawUIBenTrai();

        // Ve ma
        for (int i = 0; i < 3; i++) {
            if (ma[i].isActive) {
                drawGhost(ma[i].x, ma[i].y, ma[i].mau);
            } else {
                drawGhost(ma[i].startX, ma[i].startY, ma[i].mau);
            }
        }
        
        drawPacman(pacX, pacY, huongPac);

        setcolor(MAU_VANG); settextstyle(0, 0, 1); 
        outtextxy(50, 20, (char*)"An ESC de quay ve Menu");

        char strDiem[50]; sprintf(strDiem, (char*)"Diem so: %d", diemSo);
        setcolor(MAU_VANG); settextstyle(1, 0, 3); 
        outtextxy(50, getmaxy() - 100, strDiem);

        if (!daBatDau) {
            setcolor(MAU_VANG);
            settextstyle(3, 0, 4);
            char msgReady[] = "BAM MUI TEN DE BAT DAU!";
            outtextxy(tamX - textwidth(msgReady)/2, tamY + 50, msgReady);
        }

        setvisualpage(trangVao); 
        trangVao = 1 - trangVao; 

        // 1. NHAN TIN HIEU DIEU KHIEN
        if (kbhit()) {
            ch = getch();
            if (ch == 27) break; // Bam ESC de thoat
            if (ch == 0 || ch == 224) {
                ch = getch();
                if (ch == 72) nextHuong = 3; // Mui ten LEN
                if (ch == 80) nextHuong = 1; // Mui ten XUONG
                if (ch == 75) nextHuong = 2; // Mui ten TRAI
                if (ch == 77) nextHuong = 0; // Mui ten PHAI
                daBatDau = true; 
            }
        }

        // 2. HE THONG DI CHUYEN & VA CHAM
        if (daBatDau) {
            if (startTime == 0) startTime = clock();

            // Tha ma
            int elapsedTime = (clock() - startTime) / CLOCKS_PER_SEC;
            for (int i = 0; i < 3; i++) {
                if (!ma[i].isActive && elapsedTime >= ma[i].releaseTime) {
                    ma[i].isActive = true;
                    ma[i].x = tamX;
                    ma[i].y = tamY - 60;
                    ma[i].huong = rand() % 4; // Ngau nhien huong luc tha ra
                }
            }

            int rx = pacX - tamX;
            int ry = pacY - tamY;
            int r = 29; // Coi nhu Pacman kich thuoc 60 (ban kinh 29 de vua khit gap 60 ma khong loi)

            // Cho phep quay dau ngay lap tuc
            if (abs(huongPac - nextHuong) == 2) {
                huongPac = nextHuong;
            } 
            // Chi cho phep re khi dang o giua o grid (boi so cua 60)
            else if (rx % 60 == 0 && ry % 60 == 0 && huongPac != nextHuong) {
                int cx = rx, cy = ry;
                if (nextHuong == 0) cx += 20; 
                else if (nextHuong == 1) cy += 20;
                else if (nextHuong == 2) cx -= 20;
                else if (nextHuong == 3) cy -= 20;
                
                bool chanRe = false;
                if (nextHuong == 0) {
                    if (laVatCan(cx + r, cy - r + 1) || laVatCan(cx + r, cy + r - 1)) chanRe = true;
                } else if (nextHuong == 1) {
                    if (laVatCan(cx - r + 1, cy + r) || laVatCan(cx + r - 1, cy + r)) chanRe = true;
                } else if (nextHuong == 2) {
                    if (laVatCan(cx - r, cy - r + 1) || laVatCan(cx - r, cy + r - 1)) chanRe = true;
                } else if (nextHuong == 3) {
                    if (laVatCan(cx - r + 1, cy - r) || laVatCan(cx + r - 1, cy - r)) chanRe = true;
                }
                
                if (!chanRe) huongPac = nextHuong;
            }

            int newX = pacX, newY = pacY;
            if (huongPac == 0) newX += tocDo;      // Chay sang Phai
            else if (huongPac == 1) newY += tocDo; // Chay Xuong duoi
            else if (huongPac == 2) newX -= tocDo; // Chay sang Trai
            else if (huongPac == 3) newY -= tocDo; // Chay Len tren
            
            int nrx = newX - tamX;
            int nry = newY - tamY;
            bool vaCham = false;

            // Kiem tra va cham cho buoc tiep theo
            if (huongPac == 0) {
                if (laVatCan(nrx + r, nry - r + 1) || laVatCan(nrx + r, nry + r - 1)) vaCham = true;
            } else if (huongPac == 1) {
                if (laVatCan(nrx - r + 1, nry + r) || laVatCan(nrx + r - 1, nry + r)) vaCham = true;
            } else if (huongPac == 2) {
                if (laVatCan(nrx - r, nry - r + 1) || laVatCan(nrx - r, nry + r - 1)) vaCham = true;
            } else if (huongPac == 3) {
                if (laVatCan(nrx - r + 1, nry - r) || laVatCan(nrx + r - 1, nry - r)) vaCham = true;
            }

            if (!vaCham) {
                pacX = newX;
                pacY = newY;
            }

            // Xu ly AI ma di chuyen
            for (int i = 0; i < 3; i++) {
                if (!ma[i].isActive) continue;

                int mrx = ma[i].x - tamX;
                int mry = ma[i].y - tamY;

                if (mrx % 60 == 0 && mry % 60 == 0) {
                    int validDirs[4];
                    int count = 0;
                    for (int d = 0; d < 4; d++) {
                        if (d == (ma[i].huong + 2) % 4) continue; // Khong quay dau
                        
                        int cx = mrx, cy = mry;
                        if (d == 0) cx += 20;
                        if (d == 1) cy += 20;
                        if (d == 2) cx -= 20;
                        if (d == 3) cy -= 20;
                        
                        bool chan = false;
                        if (d == 0 && (laVatCan(cx + r, cy - r + 1) || laVatCan(cx + r, cy + r - 1))) chan = true;
                        if (d == 1 && (laVatCan(cx - r + 1, cy + r) || laVatCan(cx + r - 1, cy + r))) chan = true;
                        if (d == 2 && (laVatCan(cx - r, cy - r + 1) || laVatCan(cx - r, cy + r - 1))) chan = true;
                        if (d == 3 && (laVatCan(cx - r + 1, cy - r) || laVatCan(cx + r - 1, cy - r))) chan = true;
                        
                        if (!chan) validDirs[count++] = d;
                    }
                    
                    if (count > 0) {
                        ma[i].huong = validDirs[rand() % count];
                    } else {
                        ma[i].huong = (ma[i].huong + 2) % 4; // Duong cut thi quay dau
                    }
                }

                int tocDoMa = 20;
                if (ma[i].huong == 0) ma[i].x += tocDoMa;
                else if (ma[i].huong == 1) ma[i].y += tocDoMa;
                else if (ma[i].huong == 2) ma[i].x -= tocDoMa;
                else if (ma[i].huong == 3) ma[i].y -= tocDoMa;
            }

            // Kiem tra va cham Ma - Pacman
            bool chet = false;
            for (int i = 0; i < 3; i++) {
                if (ma[i].isActive && abs(ma[i].x - pacX) <= 30 && abs(ma[i].y - pacY) <= 30) {
                    chet = true;
                    break;
                }
            }
            if (chet) {
                hienThiGameOver(diemSo);
                break;
            }

            // 3. XU LY AN THUC AN VA CONG DIEM
            int prx = pacX - tamX;
            int pry = pacY - tamY;

            for (int i = 0; i < 15; i++) {
                for (int j = 0; j < 11; j++) {
                    if (thucAn[i][j]) {
                        int tx = -420 + i * 60;
                        int ty = -300 + j * 60;
                        if (abs(prx - tx) <= 20 && abs(pry - ty) <= 20) {
                            thucAn[i][j] = false;
                            diemSo += 10;
                        }
                    }
                }
            }

            // An vat pham phu tro
            if (giayTonTai && abs(prx - (-420)) <= 20 && abs(pry - 300) <= 20) {
                giayTonTai = false; diemSo += 50; tocDo = 30; // Tang toc do thuc te
            }
            if (thucAnLonTonTai && abs(prx - 420) <= 20 && abs(pry - 300) <= 20) {
                thucAnLonTonTai = false; diemSo += 100;
            }
            if (hoaMaTonTai && abs(prx - 0) <= 20 && abs(pry - (-300)) <= 20) {
                hoaMaTonTai = false; diemSo += 50;
            }
        }

        delay(10); 
    }
    
    setactivepage(0); 
    setvisualpage(0);
}

int main() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    initwindow(screenWidth, screenHeight, "Pac-Man Project C++", -3, -3);

    char key;
    while (true) {
        drawMenu();
        key = getch();   
        if (key == '1') { startGame(); }
        else if (key == '2') { showRules(); }
        else if (key == '3') { closegraph(); return 0; }
    }
}
