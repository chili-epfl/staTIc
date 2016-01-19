#include "backgroundsubtraction.h"
#include "backgroundsubtractionfilter.h"

BackgroundSubtraction::BackgroundSubtraction(QObject* parent):
    QAbstractVideoFilter(parent)
{
    m_entropy=0;
}

QVideoFilterRunnable* BackgroundSubtraction::createFilterRunnable(){
    BackgroundSubtractionFilter* filter= new BackgroundSubtractionFilter();
    connect(filter,SIGNAL(updated(qreal)),this,SLOT(updateEntropy(qreal)));
    return filter;
}

void BackgroundSubtraction::updateEntropy(qreal val){
    if(m_entropy!=val){
        m_entropy=val;
        emit entropyChanged();
    }
}
