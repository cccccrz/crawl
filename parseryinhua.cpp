#include "parseryinhua.h"

#include <QTextCodec>
#include <QDebug>

/*  樱花动漫详情页  */
/*  <a href="/show/273.html" title="妖精的尾巴"> */

#define TAGNAME "a"
#define ATTRNAME "href"
//#define ROOTURL "http://www.imomoe.ai"
//#define MATCHURL "/view/"
#define ROOTURL "http://www.yhdm.io"
#define MATCHURL "/show/"

extern Parser* CreateParser_YINHUA(QString html)
{
    return (new ParserYinhua(html));
}

ParserYinhua::ParserYinhua(QString html)
{
    m_dom = m_parser.parseTree(html.toStdString());
    m_it = m_dom.begin();
    m_endit = m_dom.end();

    m_tagName = TAGNAME;
    m_attrName = ATTRNAME;
    m_rootURL = ROOTURL;
    m_matchURL = MATCHURL;
}

ParserYinhua::~ParserYinhua()
{

}


//void encodeURI(QString str, QByteArray &outArr)
//{
//    QTextCodec *codec = QTextCodec::codecForName("GB2312");
//    if(codec->canEncode(str))
//    {
//        QByteArray tmpArr;
//        tmpArr = codec->fromUnicode(str);
//        for(int i=0,size = tmpArr.length();i<size;i++)
//        {
//            char ch = tmpArr.at(i);
//            if(ch < 128 && ch > 0)
//            {
//                outArr.append(ch);
//            }
//            else
//            {
//                uchar low = ch & 0xff;
//                char c[3];
//                sprintf(c,"%02X",low);
//                outArr.append("%").append(c);
//            }
//        }
//    }
//}


//QString decodeURI(QString str)
//{
//    QByteArray array;
//    for(int i=0; i<str.length();) {
//        if (0==QString::compare(str.mid(i,1), QString("%"))) {
//            if ((i+2)<str.length()) {
//                array.append(str.mid(i+1,2).toShort(0,16));
//                i=i+3;
//            }
//            else{
//                array.append(str.mid(i,1));
//                i++;
//            }
//        }
//        else{
//            array.append(str.mid(i,1));
//            i++;
//        }
//    }
//    QTextCodec *code=QTextCodec::codecForName("gb2312");
//    return code->toUnicode(array);
//}

void ParserYinhua::Parse()
{
    for (; m_it != m_endit; ++m_it) {
        if (!m_it->isTag())
            continue;
        if ((_stricmp(m_it->tagName().c_str(), m_tagName.toUtf8().data()) == 0))
        {
            //qDebug()<<"[parse]:"<<m_it->text().c_str()<<endl;
            m_it->parseAttributes(); // 扩展子标签
            // 匹配子标签
            if (m_it->attribute(m_attrName.toUtf8().data()).first)
            {
                m_resURL = m_it->attribute(m_attrName.toUtf8().data()).second.c_str();

                /* 匹配结果URL */
                if (m_resURL.toStdString().compare(0, (uint)m_matchURL.length(), m_matchURL.toStdString()) == 0)
                {
                    if(m_it->attribute("title").first) // 匹配结果标题
                    {
                        QString title = m_it->attribute("title").second.c_str();
                        QString resurl = title + " " + m_rootURL + m_resURL;
                        MyTable::GetInstance()->PushResultTable(resurl);
                    }
                }

                /* 匹配有效工作URL */
                if (m_resURL.toStdString().compare(0, 1, "/") == 0)
                {
                    m_resURL = m_rootURL + m_resURL;
                }

                /* 过滤无效URL */
                if (m_resURL.compare(m_rootURL + '/') == 0) // 去除根URL
                    continue;
                if(m_resURL.toStdString().compare(0,4,"http")!=0) // 过滤非http/https协议
                    continue;

                /* 加入任务 */
                MyTable::GetInstance()->PushTodoTable(m_resURL);
                //qDebug()<<"[TODO]:"<<m_resURL;
            }
        }
    }
}
