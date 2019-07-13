#include "Sprite.h"
#include "SDL.h"
#include "windows.h"


void Sprite::update()
{
	unsigned long timeSincelastFrame = timeGetTime() - m_TimeLastFrame;
	if(timeSincelastFrame >= m_Speed) {
		m_CurrentFrame++;
		if(m_CurrentFrame >= m_TotalFrames) 
			m_CurrentFrame = m_StartFrame;

		setFrameRect(m_CurrentFrame);

		m_TimeLastFrame = timeGetTime();
	
	}
}

void Sprite::play()
{
	auto resource = GetResource();
	SDL_Surface* TmpSurface = resource->m_Surface;
	m_FrameWidth = TmpSurface->w/m_FramesPerRow;
	m_FrameHeight = TmpSurface->h/m_FramesPerColumn;
	m_CurrentFrame = m_StartFrame;
	setFrameRect(m_CurrentFrame);
	m_TimeLastFrame = timeGetTime();
}

void Sprite::setFrameRect(unsigned int FrameNumber)
{
	unsigned int RowNumber = floor(FrameNumber/m_FramesPerRow);
	unsigned int ColumnNumber = FrameNumber;

	if(RowNumber > 0)
		ColumnNumber = FrameNumber - (RowNumber * m_FramesPerRow);
	
	GetResource()->m_Surface->clip_rect.x = ColumnNumber * m_FrameWidth;
	GetResource()->m_Surface->clip_rect.y = RowNumber * m_FrameHeight;
	GetResource()->m_Surface->clip_rect.w = m_FrameWidth;
	GetResource()->m_Surface->clip_rect.h = m_FrameHeight;
}
