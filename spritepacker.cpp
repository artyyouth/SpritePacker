#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

namespace spritepacker
{
    class CImage
    {
    public:
        CImage(int width, int height)
            : width_(width),
            height_(height),
            area_(width * height)
        {}
        virtual ~CImage() {}

        int width() const { return width_; }
        int height() const { return height_; }
        int area() const { return area_; }
    private:
        int width_;
        int height_;
        int area_;
    };

    class CSprite : public CImage
    {
    public:
        CSprite(int x, int y, int width, int height)
            : CImage(width, height),
            x_(x),
            y_(y)
        {}
        CSprite(int x, int y, const CImage& img)
            : CImage(img.width(), img.height()),
            x_(x),
            y_(y)
        {

        }
        ~CSprite() {}

        int x() const { return x_; }
        int y() const { return y_; }

    private:
        int x_;
        int y_;
    };

    class CSpriteSheet
    {
    public:
        CSpriteSheet(int width, int height)
            : width_(width),
            height_(height)
        {
            rowHeights_.push_back(height);
            colWidths_.push_back(width);
            occupied_.resize(1);
            occupied_[0] = false;
        }

        ~CSpriteSheet() 
        {

        }

        bool occupied(int row, int col) const 
        { 
            int index = row * colSize() + col;
            if (index < occupied_.size())
            {
                return occupied_[index];
            }

            return false;
        }

        int rowSize() const { return rowHeights_.size(); }
        int colSize() const { return colWidths_.size(); }
        int rowHeight(int r) const { return rowHeights_[r]; }
        int colWidth(int c) const { return colWidths_[c]; }

        const vector<CSprite>& sprites() const { return sprites_; }
        void putImages(vector<CImage>& images)
        {
            int i = 0;

            // put as many images as possible into current sprite sheet
            while (i < images.size())
            {
                if (putSingleImage(images[i]))
                {
                    // remove image that already be put into current sprite sheet
                    images.erase(images.begin() + i);
                }
                else
                {
                    // pass the image that can not be put into current sprite sheet
                    i++;
                }
            }

            return;
        }

        bool putSingleImage(const CImage& image)
        {
            int x = 0;
            int y = 0;

            for (int i = 0; i < rowSize(); i++)
            {
                x = 0;
                // search through rows/columns to find a empty slot with enough
                // area to put image
                if (rowHeight(i) >= image.height())
                {
                    int occupiedWidth = 0;
                    for (int j = 0; j < colSize(); j++)
                    {
                        if (occupied(i, j))
                        {
                            occupiedWidth += colWidth(j);
                        }
                        else
                        {
                            int remainingWidth = width_ - occupiedWidth;
                            // a valid slot is found
                            if (remainingWidth >= image.width())
                            {
                                // put image
                                CSprite sprite(x, y, image);
                                sprites_.push_back(sprite);

                                // insert area to update the occupied matrix information
                                insertArea(i, j, image.width(), image.height());
                                // printOccupiedAreas();
                                return true;
                            }
                        }

                        x += colWidth(j);
                    }
                }

                y += rowHeight(i);
            }

            // search through the entire sprite sheet but there is not
            // any valid slot with enough area to put image
            return false;
        }

        void printOccupiedAreas()
        {
            for (int i = 0; i < rowSize(); i++)
            {
                for (int j = 0; j < colSize(); j++)
                {
                    cout << (occupied_[i * colSize() + j] ? 1 : 0);
                }
                cout << endl;
            }
            cout << endl;
        }

    private:
        vector<CSprite> sprites_;
        int width_;
        int height_;
        vector<bool> occupied_;
        vector<int> rowHeights_;
        vector<int> colWidths_;

        void insertArea(int row, int col, int width, int height)
        {
            // update col widths
            updateDimension(colWidths_, col, width);

            // update row heights
            updateDimension(rowHeights_, row, height);

            // update occupied matrix
            int r = rowSize();
            int c = colSize();
            occupied_.resize(r * c);
            int x = 0;
            int y = 0;
            int w = 0;
            int h = 0;

            for (int i = 0; i < r; i++)
            {
                x = 0;
                h = rowHeight(i);
                for (int j = 0; j < c; j++)
                {
                    occupied_[i * c + j] = false;
                    w = colWidth(j);
                    
                    int spriteSize = sprites_.size();
                    for (int k = 0; k < spriteSize; k++)
                    {
                        // intercet detection
                        CSprite sprite = sprites_[k];
                        if(!((x >= (sprite.x() + sprite.width())) || (x + w < sprite.x()) || 
                            (y >= (sprite.y() + sprite.height())) || (y + h < sprite.y())))
                        {
		                    occupied_[i * c + j] = true;
                            break;
                        }
                    }
                    x += colWidth(j);
                }
                y += rowHeight(i);
            }
        }

        void updateDimension(vector<int>& arr, int index, int newValue)
        {
            int curValue = arr[index];

            if (newValue > curValue)
            {
                int i = index + 1;
                int restValue = newValue - curValue;
                while (restValue - arr[i] > 0)
                {
                    restValue -= arr[i];
                    i++;
                }
                if (restValue != 0)
                {
                    arr[i] -= restValue;
                    arr.insert(arr.begin() + i, restValue);
                }
            }
            else if (newValue < curValue)
            {
                arr[index] -= newValue;
                arr.insert(arr.begin() + index, newValue);
            }
        }
    };

    bool parseInput(int argc, const char** argv, vector<CImage>& images)
    {
        if (argc <= 1) return false;

        for (int i = 1; i < argc; i++)
        {
            string imgDef(argv[i]);
            string::size_type loc = imgDef.find("x", 0);

            if (loc != imgDef.npos)
            {
                string widthStr = imgDef.substr(0, loc);
                string heightStr = imgDef.substr(loc + 1, imgDef.length() - loc - 1);
                int width = atoi(widthStr.c_str());
                int height = atoi(heightStr.c_str());
                if (width == 0 || height == 0) return false;
                CImage img(width, height);
                images.push_back(img);
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    void printResult(const vector<CSpriteSheet>& spriteSheets)
    {
        int sheetSize = spriteSheets.size();
        for (int i = 0; i < sheetSize; i++)
        {
            cout << "sheet " << (i + 1) << endl;
            const CSpriteSheet& sheet = spriteSheets[i];
            const vector<CSprite>& sprites = sheet.sprites();
            int spriteSize = sprites.size();
            for (int j = 0; j < spriteSize; j++)
            {
                cout << sprites[j].width() << "x" << sprites[j].height()
                    << " " << sprites[j].x() << " " << sprites[j].y() << endl;
            }
            cout << endl;
        }
    }

    void sortImagesByHeight(vector<CImage>& images)
    {
        int imgSize = images.size();

        for (int i = 0; i < imgSize; i++)
        {
            for (int j = i + 1; j < imgSize; j++)
            {
                if (images[i].height() < images[j].height())
                {
                    CImage tmp = images[i];
                    images[i] = images[j];
                    images[j] = tmp;
                }
            }
        }
    }
}

using namespace spritepacker;

int main(int argc, const char* argv[])
{
    vector<CImage> images;

    if (!parseInput(argc, argv, images) || images.size() == 0)
    {
        cout << "Failed to parse the input." << endl;
        return 1;
    }

    sortImagesByHeight(images);

    vector<CSpriteSheet> spriteSheets;

    while (images.size() > 0)
    {
        CSpriteSheet sheet(1024, 1024);
        sheet.putImages(images);
        spriteSheets.push_back(sheet);
    }

    printResult(spriteSheets);
    getchar();
	return 0;
}