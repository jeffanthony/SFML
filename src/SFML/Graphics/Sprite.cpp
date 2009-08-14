////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/GLCheck.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
Sprite::Sprite() :
mySubRect   (0, 0, 1, 1),
myIsFlippedX(false),
myIsFlippedY(false)
{

}


////////////////////////////////////////////////////////////
/// Construct the sprite from a source image
////////////////////////////////////////////////////////////
Sprite::Sprite(const Image& image, const Vector2f& position, const Vector2f& scale, float rotation, const Color& color) :
Drawable    (position, scale, rotation, color),
mySubRect   (0, 0, 1, 1),
myIsFlippedX(false),
myIsFlippedY(false)
{
    SetImage(image);
}


////////////////////////////////////////////////////////////
/// Set the image of the sprite
////////////////////////////////////////////////////////////
void Sprite::SetImage(const Image& image, bool adjustToNewSize)
{
    // If there was no valid image before, force adjusting to the new image size
    if (!myImage)
        adjustToNewSize = true;

    // If we want to adjust the size and the new image is valid, we adjust the source rectangle
    if (adjustToNewSize && (image.GetWidth() > 0) && (image.GetHeight() > 0))
    {
        SetSubRect(IntRect(0, 0, image.GetWidth(), image.GetHeight()));
    }

    // Assign the new image
    myImage = &image;
}


////////////////////////////////////////////////////////////
/// Set the sub-rectangle of the sprite inside the source image
////////////////////////////////////////////////////////////
void Sprite::SetSubRect(const IntRect& rectangle)
{
    mySubRect = rectangle;
}


////////////////////////////////////////////////////////////
/// Resize the sprite (by changing its scale factors) (take 2 values).
/// The default size is defined by the subrect
////////////////////////////////////////////////////////////
void Sprite::Resize(float width, float height)
{
    int localWidth  = mySubRect.GetSize().x;
    int localHeight = mySubRect.GetSize().y;

    if ((localWidth > 0) && (localHeight > 0))
        SetScale(width / localWidth, height / localHeight);
}


////////////////////////////////////////////////////////////
/// Resize the object (by changing its scale factors) (take a 2D vector)
/// The default size is defined by the subrect
////////////////////////////////////////////////////////////
void Sprite::Resize(const Vector2f& size)
{
    Resize(size.x, size.y);
}


////////////////////////////////////////////////////////////
/// Flip the sprite horizontally
////////////////////////////////////////////////////////////
void Sprite::FlipX(bool flipped)
{
    myIsFlippedX = flipped;
}


////////////////////////////////////////////////////////////
/// Flip the sprite vertically
////////////////////////////////////////////////////////////
void Sprite::FlipY(bool flipped)
{
    myIsFlippedY = flipped;
}


////////////////////////////////////////////////////////////
/// Get the source image of the sprite
////////////////////////////////////////////////////////////
const Image* Sprite::GetImage() const
{
    return myImage;
}


////////////////////////////////////////////////////////////
/// Get the sub-rectangle of the sprite inside the source image
////////////////////////////////////////////////////////////
const IntRect& Sprite::GetSubRect() const
{
    return mySubRect;
}


////////////////////////////////////////////////////////////
/// Get the sprite size
////////////////////////////////////////////////////////////
Vector2f Sprite::GetSize() const
{
    return Vector2f(mySubRect.GetSize().x * GetScale().x, mySubRect.GetSize().y * GetScale().y);
}


////////////////////////////////////////////////////////////
/// Get the color of a given pixel in the sprite
/// (point is in local coordinates)
////////////////////////////////////////////////////////////
Color Sprite::GetPixel(unsigned int x, unsigned int y) const
{
    if (myImage)
    {
        unsigned int imageX = mySubRect.Left + x;
        unsigned int imageY = mySubRect.Top  + y;

        if (myIsFlippedX) imageX = mySubRect.GetSize().x - imageX - 1;
        if (myIsFlippedY) imageY = mySubRect.GetSize().y - imageY - 1;

        return myImage->GetPixel(imageX, imageY) * GetColor();
    }
    else
    {
        return GetColor();
    }
}


////////////////////////////////////////////////////////////
/// /see Drawable::Render
////////////////////////////////////////////////////////////
void Sprite::Render(RenderTarget&) const
{
    // Get the sprite size
    float width  = static_cast<float>(mySubRect.GetSize().x);
    float height = static_cast<float>(mySubRect.GetSize().y);

    // Check if the image is valid
    if (myImage && (myImage->GetWidth() > 0) && (myImage->GetHeight() > 0))
    {
        // Use the "offset trick" to get pixel-perfect rendering
        // see http://www.opengl.org/resources/faq/technical/transformations.htm#tran0030
        GLCheck(glTranslatef(0.375f, 0.375f, 0.f));

        // Bind the texture
        myImage->Bind();

        // Calculate the texture coordinates
        FloatRect texCoords = myImage->GetTexCoords(mySubRect);
        FloatRect rect(myIsFlippedX ? texCoords.Right  : texCoords.Left,
                       myIsFlippedY ? texCoords.Bottom : texCoords.Top,
                       myIsFlippedX ? texCoords.Left   : texCoords.Right,
                       myIsFlippedY ? texCoords.Top    : texCoords.Bottom);

        // Draw the sprite's triangles
        glBegin(GL_QUADS);
            glTexCoord2f(rect.Left,  rect.Top);    glVertex2f(0,     0);
            glTexCoord2f(rect.Left,  rect.Bottom); glVertex2f(0,     height);
            glTexCoord2f(rect.Right, rect.Bottom); glVertex2f(width, height);
            glTexCoord2f(rect.Right, rect.Top);    glVertex2f(width, 0) ;
        glEnd();
    }
    else
    {
        // Disable texturing
        GLCheck(glDisable(GL_TEXTURE_2D));

        // Draw the sprite's triangles
        glBegin(GL_QUADS);
            glVertex2f(0,     0);
            glVertex2f(0,     height);
            glVertex2f(width, height);
            glVertex2f(width, 0);
        glEnd();
    }
}

} // namespace sf
