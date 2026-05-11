#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>

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

// 1. Thuat toan Bresenham ve doan thang
void veDuongThang(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int stepX = (x1 < x2) ? 1 : -1;
    int stepY = (y1 < y2) ? 1 : -1;
    int p = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (true) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = p;
        if (e2 > -dx) { p -= dy; x1 += stepX; }
        if (e2 < dy) { p += dx; y1 += stepY; }
    }
}

// 2. Ham phu tro ve Hinh Chu Nhat dua tren Bresenham
void veHinhChuNhat(int left, int top, int right, int bottom, int color) {
    veDuongThang(left, top, right, top, color);       // Canh tren
    veDuongThang(right, top, right, bottom, color);   // Canh phai
    veDuongThang(right, bottom, left, bottom, color); // Canh duoi
    veDuongThang(left, bottom, left, top, color);     // Canh trai
}

// 3. Thuat toan Midpoint ve Duong tron
void veDuongTron(int xc, int yc, int r, int color) {
    int x = 0, y = r;
    int p = 1 - r;

    while (x <= y) {
        putpixel(xc + x, yc + y, color);
        putpixel(xc - x, yc + y, color);
        putpixel(xc + x, yc - y, color);
        putpixel(xc - x, yc - y, color);
        putpixel(xc + y, yc + x, color);
        putpixel(xc - y, yc + x, color);
        putpixel(xc + y, yc - x, color);
        putpixel(xc - y, yc - x, color);

        x++;
        if (p < 0) { p += 2 * x + 1; } 
        else { y--; p += 2 * (x - y) + 1; }
    }
}

// 4. Thuat toan To mau De quy (Flood Fill - 4 huong)
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

// Ve Pacman
void drawPacman(int x, int y, int huong) {
    int r = 20;
    for(int i = y - 21; i <= y + 21; i++) {
        veDuongThang(x - 21, i, x + 21, i, MAU_DEN);
    }

    // Buoc 1: Ve vien hinh tron
    veDuongTron(x, y, r, MAU_VANG);

    int mx1, my1, mx2, my2; 
    int xLung, yLung;       

    if (huong == 0) { // PHAI
        mx1 = x + 16; my1 = y - 12; mx2 = x + 16; my2 = y + 12;
        xLung = x - 10; yLung = y;  
    } else if (huong == 2) { // TRAI
        mx1 = x - 16; my1 = y - 12; mx2 = x - 16; my2 = y + 12;
        xLung = x + 10; yLung = y;  
    } else if (huong == 1) { // XUONG
        mx1 = x - 12; my1 = y + 16; mx2 = x + 12; my2 = y + 16;
        xLung = x; yLung = y - 10;  
    } else if (huong == 3) { // LEN
        mx1 = x - 12; my1 = y - 16; mx2 = x + 12; my2 = y - 16;
        xLung = x; yLung = y + 10;  
    }

    // Buoc 2: Ve mieng
    veDuongThang(x, y, mx1, my1, MAU_VANG);
    veDuongThang(x, y, mx2, my2, MAU_VANG);

    // Buoc 3: To mau than Pac-man
    toMauDeQuy(xLung, yLung, MAU_VANG, MAU_DEN);

    if (huong == 0) {
        for(int i = x + 16; i <= x + 21; i++) veDuongThang(i, y - 12, i, y + 12, MAU_DEN);
    } else if (huong == 2) {
        for(int i = x - 21; i <= x - 16; i++) veDuongThang(i, y - 12, i, y + 12, MAU_DEN);
    } else if (huong == 1) {
        for(int i = y + 16; i <= y + 21; i++) veDuongThang(x - 12, i, x + 12, i, MAU_DEN);
    } else if (huong == 3) {
        for(int i = y - 21; i <= y - 16; i++) veDuongThang(x - 12, i, x + 12, i, MAU_DEN);
    }
}

// Ve Ma
void drawGhost(int x, int y, int color) {
    for(int i = y - 16; i <= y + 21; i++) {
        veDuongThang(x - 16, i, x + 16, i, MAU_DEN);
    }

    // BUOC 1: VE BIEN
    veDuongTron(x, y, 15, color); 
    veDuongThang(x - 15, y, x - 15, y + 20, color);      
    veDuongThang(x + 15, y, x + 15, y + 20, color);      
    veDuongThang(x - 15, y + 20, x + 15, y + 20, color); 

    // BUOC 2: TO MAU THAN MA
    toMauDeQuy(x, y, color, MAU_DEN); 
    toMauDeQuy(x, y + 18, color, MAU_DEN); 

    // BUOC 3: VE MAT
    veDuongTron(x - 6, y + 3, 4, MAU_TRANG); 
    toMauDeQuy(x - 6, y + 3, MAU_TRANG, color); 

    veDuongTron(x + 6, y + 3, 4, MAU_TRANG); 
    toMauDeQuy(x + 6, y + 3, MAU_TRANG, color);

    // BUOC 4: VE CON NGUOI
    veDuongTron(x - 8, y + 3, 1, MAU_XANH_DUONG); 
    putpixel(x - 8, y + 3, MAU_XANH_DUONG);
    veDuongTron(x + 4, y + 3, 1, MAU_XANH_DUONG); 
    putpixel(x + 4, y + 3, MAU_XANH_DUONG);
}

// Ve thuc an
void drawFood(int x, int y) {
    veDuongTron(x, y, 3, MAU_TRANG);
    toMauDeQuy(x, y, MAU_TRANG, MAU_DEN); // To mau lap day vien thuc an
}

// Ve Phu tro
void vePhuTro(int x, int y, int loai) {
    int banKinhLon = 18;
    
    if (loai == 1) { 
        // 1. PHU TRO GIAY
        veDuongTron(x, y, banKinhLon, MAU_XANH_LA);
        setcolor(MAU_XANH_LA); settextstyle(2, 0, 4); 
        outtextxy(x - 14, y - 7, (char*)"GIAY"); 
    } 
    else if (loai == 2) { 
        // 2. THUC AN LON
        veDuongTron(x, y, banKinhLon, MAU_VANG);
        veDuongTron(x, y, 7, MAU_VANG);
        toMauDeQuy(x, y, MAU_VANG, MAU_DEN); 
    }
    else if (loai == 3) { 
        // 3. HOA MA
        veDuongTron(x, y, banKinhLon, MAU_CYAN);
        
        int my = y - 2; 
        
        veDuongTron(x, my, 8, MAU_TRANG);                      // Dau ma
        veDuongThang(x - 8, my, x - 8, my + 10, MAU_TRANG);    // Canh trai
        veDuongThang(x + 8, my, x + 8, my + 10, MAU_TRANG);    // Canh phai
        veDuongThang(x - 8, my + 10, x + 8, my + 10, MAU_TRANG); // Canh day

        toMauDeQuy(x, my, MAU_TRANG, MAU_DEN);     // To phan dau (trong hinh tron)
        toMauDeQuy(x, my + 9, MAU_TRANG, MAU_DEN); // To phan than (duoi day hinh tron, thong 2 goc)

        //Ve 2 con mat mau den
        veDuongTron(x - 3, my + 2, 1, MAU_DEN);
        putpixel(x - 3, my + 2, MAU_DEN);
        
        veDuongTron(x + 3, my + 2, 1, MAU_DEN);
        putpixel(x + 3, my + 2, MAU_DEN);
    }
}

void veKhoi(int tamX, int tamY, int x1, int y1, int x2, int y2) {
    for(int i = y1 + 1; i < y2; i++) {
        veDuongThang(tamX + x1 + 1, tamY + i, tamX + x2 - 1, tamY + i, MAU_DEN);
    }
    // Ve vien chu nhat bang thuat toan Bresenham
    veHinhChuNhat(tamX + x1, tamY + y1, tamX + x2, tamY + y2, MAU_XANH_NGOC);
}

// HAM VE MAP
void drawMaze() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;
    
    // Khung vien ngoai (Bresenham)
    veHinhChuNhat(tamX - 450, tamY - 330, tamX + 450, tamY + 330, MAU_XANH_NGOC);
    veHinhChuNhat(tamX - 455, tamY - 335, tamX + 455, tamY + 335, MAU_XANH_NGOC);

    // Chuong ma o giua
    veKhoi(tamX, tamY, -90, -30, 90, 30);
    veDuongThang(tamX - 40, tamY - 30, tamX + 40, tamY - 30, MAU_VANG); // Cua chuong

    // 1. KHOI CHU T (Tren va Duoi)
    // T Tren
    veKhoi(tamX, tamY, -150, -270, 150, -210); // Thanh ngang
    veKhoi(tamX, tamY, -30, -210, 30, -150);   // Thanh doc
    veDuongThang(tamX - 29, tamY - 210, tamX + 29, tamY - 210, MAU_DEN); // Tay vien noi
    
    // T Duoi
    veKhoi(tamX, tamY, -150, 210, 150, 270); 
    veKhoi(tamX, tamY, -30, 150, 30, 210);   
    veDuongThang(tamX - 29, tamY + 210, tamX + 29, tamY + 210, MAU_DEN);

    // 2. KHOI CHU L
    // L Goc Trai Tren
    veKhoi(tamX, tamY, -390, -270, -330, -150); 
    veKhoi(tamX, tamY, -330, -270, -210, -210); 
    veDuongThang(tamX - 330, tamY - 269, tamX - 330, tamY - 211, MAU_DEN);

    // L Goc Phai Tren
    veKhoi(tamX, tamY, 330, -270, 390, -150); 
    veKhoi(tamX, tamY, 210, -270, 330, -210); 
    veDuongThang(tamX + 330, tamY - 269, tamX + 330, tamY - 211, MAU_DEN);

    // L Goc Trai Duoi
    veKhoi(tamX, tamY, -390, 150, -330, 270); 
    veKhoi(tamX, tamY, -330, 210, -210, 270); 
    veDuongThang(tamX - 330, tamY + 211, tamX - 330, tamY + 269, MAU_DEN);

    // L Goc Phai Duoi
    veKhoi(tamX, tamY, 330, 150, 390, 270); 
    veKhoi(tamX, tamY, 210, 210, 330, 270); 
    veDuongThang(tamX + 330, tamY + 211, tamX + 330, tamY + 269, MAU_DEN);

    // 3. KHOI BAM MEP NGOAI
    veKhoi(tamX, tamY, -450, -90, -330, -30); // Trai tren
    veKhoi(tamX, tamY, -450, 30, -330, 90);   // Trai duoi
    veKhoi(tamX, tamY, 330, -90, 450, -30);   // Phai tren
    veKhoi(tamX, tamY, 330, 30, 450, 90);     // Phai duoi

    // 4. CAC KHOI CHU NHAT CON LAI O GIUA MAP
    veKhoi(tamX, tamY, -270, -150, -150, -90); // Giua trai tren
    veKhoi(tamX, tamY, 150, -150, 270, -90);   // Giua phai tren
    veKhoi(tamX, tamY, -270, 90, -150, 150);   // Giua trai duoi
    veKhoi(tamX, tamY, 150, 90, 270, 150);     // Giua phai duoi
    
    veKhoi(tamX, tamY, -270, -30, -150, 30); 
    veKhoi(tamX, tamY, 150, -30, 270, 30);   

    // Khoi ngan ngang tren va duoi chuong ma
    veKhoi(tamX, tamY, -90, -150, 90, -90);  
    veDuongThang(tamX - 29, tamY - 150, tamX + 29, tamY - 150, MAU_DEN);
    
    veKhoi(tamX, tamY, -90, 90, 90, 150);    
    veDuongThang(tamX - 29, tamY + 150, tamX + 29, tamY + 150, MAU_DEN);
}

// Ham ho tro kiem tra toa do (x, y) co nam trong ranh gioi cua mot hinh chu nhat khong
bool isInside(int x, int y, int x1, int y1, int x2, int y2) {
    return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}

// HAM KIEM TRA VAT CAN 
bool laVatCan(int x, int y) {
    if (isInside(x, y, -90, -30, 90, 30)) return true;       // Chuong ma o giua
    
    // Khoi chu T (Tren & Duoi)
    if (isInside(x, y, -150, -270, 150, -210)) return true;  // T tren (thanh ngang)
    if (isInside(x, y, -30, -210, 30, -150)) return true;    // T tren (thanh doc)
    if (isInside(x, y, -150, 210, 150, 270)) return true;    // T duoi (thanh ngang)
    if (isInside(x, y, -30, 150, 30, 210)) return true;      // T duoi (thanh doc)

    // Khoi chu L (4 goc)
    if (isInside(x, y, -390, -270, -330, -150)) return true; // L trai tren (doc)
    if (isInside(x, y, -330, -270, -210, -210)) return true; // L trai tren (ngang)
    if (isInside(x, y, 330, -270, 390, -150)) return true;   // L phai tren (doc)
    if (isInside(x, y, 210, -270, 330, -210)) return true;   // L phai tren (ngang)
    if (isInside(x, y, -390, 150, -330, 270)) return true;   // L trai duoi (doc)
    if (isInside(x, y, -330, 210, -210, 270)) return true;   // L trai duoi (ngang)
    if (isInside(x, y, 330, 150, 390, 270)) return true;     // L phai duoi (doc)
    if (isInside(x, y, 210, 210, 330, 270)) return true;     // L phai duoi (ngang)

    // Cac khoi bam sat mep (Edge blocks)
    if (isInside(x, y, -450, -90, -330, -30)) return true;   // Bam mep trai tren
    if (isInside(x, y, -450, 30, -330, 90)) return true;     // Bam mep trai duoi
    if (isInside(x, y, 330, -90, 450, -30)) return true;     // Bam mep phai tren
    if (isInside(x, y, 330, 30, 450, 90)) return true;       // Bam mep phai duoi

    // Cac khoi chu nhat tieu chuan o giua
    if (isInside(x, y, -270, -150, -150, -90)) return true;  // Giua trai tren
    if (isInside(x, y, 150, -150, 270, -90)) return true;    // Giua phai tren
    if (isInside(x, y, -270, 90, -150, 150)) return true;    // Giua trai duoi
    if (isInside(x, y, 150, 90, 270, 150)) return true;      // Giua phai duoi
    
    if (isInside(x, y, -270, -30, -150, 30)) return true;    // Giua trai (ngang chuong ma)
    if (isInside(x, y, 150, -30, 270, 30)) return true;      // Giua phai (ngang chuong ma)
    if (isInside(x, y, -90, -150, 90, -90)) return true;     // Tren chuong ma (noi voi chu T)
    if (isInside(x, y, -90, 90, 90, 150)) return true;

    return false; // Neu khong trung toa do nao thi la duong di an toan
}

// HAM RAI THUC AN
void drawAllFood() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;

    // Quet toan bo luoi ban do
    for (int x = -420; x <= 420; x += 60) {
        for (int y = -300; y <= 300; y += 60) {
            
            if (x == -420 && y == 300) continue;
            if (x == 420 && y == 300) continue;
            if (x == 0 && y == -300) continue;

            if (!laVatCan(x, y)) {
                drawFood(tamX + x, tamY + y); 
            }
        }
    }

    vePhuTro(tamX - 420, tamY + 300, 1); 
    vePhuTro(tamX + 420, tamY + 300, 2); 
    vePhuTro(tamX, tamY - 300, 3); 
}
void drawUIBenTrai() {
    int lX = 40; 
    int topY = 100; 

    setcolor(MAU_VANG); settextstyle(1, 0, 2); 
    outtextxy(lX, topY, (char*)"1. DIEU KHIEN:");

    setcolor(MAU_TRANG); settextstyle(2, 0, 4); 
    
    // Dung Bresenham de ve cac nut bam
    veHinhChuNhat(lX + 80, topY + 40, lX + 130, topY + 90, MAU_TRANG);
    outtextxy(lX + 92, topY + 58, (char*)"LEN");
    
    veHinhChuNhat(lX + 20, topY + 100, lX + 70, topY + 150, MAU_TRANG);
    outtextxy(lX + 26, topY + 118, (char*)"TRAI");
    
    veHinhChuNhat(lX + 80, topY + 100, lX + 130, topY + 150, MAU_TRANG);
    outtextxy(lX + 84, topY + 118, (char*)"XUONG");
    
    veHinhChuNhat(lX + 140, topY + 100, lX + 190, topY + 150, MAU_TRANG);
    outtextxy(lX + 147, topY + 118, (char*)"PHAI");

    setcolor(MAU_VANG); settextstyle(1, 0, 2);
    outtextxy(lX, topY + 180, (char*)"2. VAT PHAM:");

    vePhuTro(lX + 30, topY + 250, 1); 
    setcolor(MAU_TRANG); settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 238, (char*)"- Giay: Tang toc do (5s)");

    vePhuTro(lX + 30, topY + 320, 2);
    setcolor(MAU_TRANG); settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 308, (char*)"- Thuc an lon: +Nhieu diem (10s)");

    vePhuTro(lX + 30, topY + 390, 3);
    setcolor(MAU_TRANG); settextstyle(3, 0, 2); 
    outtextxy(lX + 70, topY + 378, (char*)"- Hoa ma: An lai duoc ma (3s)");
}

void startGame() {
    int tamX = getmaxx() / 2 + 150;
    int tamY = getmaxy() / 2;
    
    pacX = tamX; pacY = tamY + 300; huongPac = 0;
    int ch; int trangVao = 0; int diemSo = 0;
    
    time_t thoiGianBatDau = time(NULL); 
    
    time_t mocCongDiemCuoi = time(NULL); 
    srand(time(NULL));

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

        setcolor(MAU_VANG); settextstyle(0, 0, 1); 
        outtextxy(50, 20, (char*)"An ESC de quay ve Menu");

        time_t thoiGianHienTai = time(NULL);

        if (difftime(thoiGianHienTai, mocCongDiemCuoi) >= 1) {
            int diemNgauNhien = rand() % 3 + 1;
            diemSo += diemNgauNhien;
            mocCongDiemCuoi = thoiGianHienTai;
        }

        char strDiem[50]; sprintf(strDiem, (char*)"Diem so: %d", diemSo);
        setcolor(MAU_VANG); settextstyle(1, 0, 3); 
        outtextxy(50, getmaxy() - 100, strDiem);

        int thoiGianConLai = 20 - difftime(thoiGianHienTai, thoiGianBatDau); 
        
        char strThoiGian[50]; sprintf(strThoiGian, (char*)"Thoi gian: %d s", thoiGianConLai);
        setcolor(MAU_TRANG); settextstyle(1, 0, 3); 
        outtextxy(getmaxx() - 250, 20, strThoiGian);

        if (thoiGianConLai <= 0) {
            setcolor(MAU_VANG); settextstyle(1, 0, 8); 
            char msgThua[] = "THAT BAI";
            outtextxy(tamX - textwidth(msgThua)/2, tamY - textheight(msgThua)/2 - 50, msgThua);

            setcolor(MAU_TRANG); settextstyle(3, 0, 4);
            char msgThoat[] = "Ban thua roi - An phim bat ky de quay lai...";
            outtextxy(tamX - textwidth(msgThoat)/2, tamY + 50, msgThoat);

            setvisualpage(trangVao); getch(); break;   
        }

        setvisualpage(trangVao); trangVao = 1 - trangVao; 

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
    setactivepage(0); setvisualpage(0);
}

// HAM VE MENU
void drawMenu() {
    cleardevice();
    setbkcolor(MAU_DEN);

    int tamX = getmaxx() / 2; 
    int tamY = getmaxy() / 2;
    int kTrai = tamX - 350, kPhai = tamX + 350, kTren = tamY - 250, kDuoi = tamY + 250;

    // Khung vien Menu
    veHinhChuNhat(kTrai, kTren, kPhai, kDuoi, MAU_XANH_DUONG); 
    veHinhChuNhat(kTrai + 10, kTren + 10, kPhai - 10, kDuoi - 10, MAU_XANH_DUONG); 

    // Tieu de Game
    setcolor(MAU_VANG); 
    settextstyle(1, 0, 7); 
    char title[] = "PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, kTren + 40, title);

    // Cac tuy chon Menu
    settextstyle(3, 0, 5); 
    setcolor(MAU_TRANG);
    char m1[] = "1. BAT DAU CHOI"; 
    char m2[] = "2. HUONG DAN"; 
    char m3[] = "3. THOAT GAME";

    int yMenu = tamY - 30; 
    outtextxy(tamX - textwidth(m1)/2, yMenu, m1);
    outtextxy(tamX - textwidth(m2)/2, yMenu + 70, m2);
    outtextxy(tamX - textwidth(m3)/2, yMenu + 140, m3);

    int pacSize = 30;
    int pacYM = yMenu + textheight(m1)/2; 
    setfillstyle(SOLID_FILL, MAU_VANG);
    pieslice(tamX - textwidth(m1)/2 - 70, pacYM, 30, 330, pacSize); 
    pieslice(tamX + textwidth(m1)/2 + 70, pacYM, 210, 150, pacSize); 
}

// HAM HIEN THI LUAT CHOI
void showRules() {
    cleardevice(); 
    setbkcolor(MAU_DEN);

    int tamX = getmaxx() / 2; 
    int tamY = getmaxy() / 2;
    int kTrai = tamX - 500, kPhai = tamX + 500, kTren = tamY - 280, kDuoi = tamY + 280;

    // Khung vien
    veHinhChuNhat(kTrai, kTren, kPhai, kDuoi, MAU_TRANG);
    veHinhChuNhat(kTrai + 10, kTren + 10, kPhai - 10, kDuoi - 10, MAU_TRANG);

    setcolor(MAU_XANH_NGOC); 
    settextstyle(1, 0, 5); 
    char title[] = "LUAT CHOI PAC-MAN";
    outtextxy(tamX - textwidth(title)/2, kTren + 30, title);

    settextstyle(3, 0, 3); 
    int xT = kTrai + 40; 
    int yT = kTren + 110; 
    int gD = 45;

    setcolor(MAU_TRANG);
    outtextxy(xT, yT, (char*)"1. Dung mui ten de di chuyen Pac-Man.");
    outtextxy(xT, yT + gD*1, (char*)"2. Pac-Man chi dung lai khi gap vat can.");
    outtextxy(xT, yT + gD*2, (char*)"3. Nuot thuc an tren duong de cong diem.");
    outtextxy(xT, yT + gD*3, (char*)"4. Tranh ma de khong bi mat mang.");
    outtextxy(xT, yT + gD*4, (char*)"5. Cac vat pham phu tro:");
    
    int xS = xT + 40; 
    setcolor(MAU_VANG); 
    outtextxy(xS, yT + gD*5, (char*)"- Giay: Tang toc do di chuyen (5s).");
    outtextxy(xS, yT + gD*6, (char*)"- Thuc an lon: Diem to hon (10s).");
    outtextxy(xS, yT + gD*7, (char*)"- Hoa ma: Co the an lai duoc ma (3s).");

    setcolor(MAU_XANH_NGOC); 
    settextstyle(3, 0, 3);
    char footer[] = "An phim bat ky de quay lai Menu...";
    outtextxy(tamX - textwidth(footer)/2, kDuoi - 45, footer);

    getch();
}

int main() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    initwindow(screenWidth, screenHeight, "Pac-Man Game", -3, -3);

    char key;
    while (true) {
        drawMenu();
        key = getch();   
        if (key == '1') { startGame(); }
        else if (key == '2') { showRules(); }
        else if (key == '3') { closegraph(); return 0; }
    }
}
