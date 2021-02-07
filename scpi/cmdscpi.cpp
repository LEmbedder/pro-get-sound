/**
 * @file cmdscpi.cpp
 * @author Lembedder (814699480@qq.cpm)
 * @brief
 * @version 0.1
 * @date 2021-01-04
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "sysscpi.h"

void tSysScpi::handleScpiCommand(QString cmdStr, int socketIndex)
{
    QString tempStr = cmdStr;

    if (tempStr.isNull() || tempStr.isEmpty() || tempStr.length() > 65535)
	{
		return;
	}

	for (int i = 0; i < tempStr.length(); i++)
	{
		if (tempStr.at(i) == QChar(SCPIEOFCHAR[0]) || tempStr.at(i) == QChar(SCPIEOFCHAR[1]))
		{
			tempStr.replace(i, 1, ";");
		}
	}

	QStringList cmdList = tempStr.split(";");

	if (tempStr.at(tempStr.length() - 1) != QChar(';'))
	{
		if (cmdList.count() <= 1)
		{
            buffer = tempStr.toLocal8Bit();

		} else
        {
            buffer = cmdList.at(cmdList.count() - 1).toLocal8Bit();
		}

		cmdList.removeAt(cmdList.count() - 1);
	} else
	{
        buffer = "";
	}

	QString cmdContent = "";
	bool isEmpty = false;

	while (true)
	{
		isEmpty = false;

		for (int i = 0; i < cmdList.count(); i++)
		{
			cmdContent = cmdList.at(i).trimmed();

			if (cmdContent.isNull() || cmdContent.isEmpty())
			{
				isEmpty = true;
				cmdList.removeAt(i);
				break;
			}
		}

		if (!isEmpty)
		{
			break;
		}
	}

	QString command = "";
	QString returnString = "";
	resultDef result;
	int exeResult = __SCPI_FAILED;
    QString str = "";
	bool dscSegValid = false;

	for (int i = 0; i < cmdList.count(); i++)
	{
		command = cmdList.at(i).trimmed();

		if (command.isNull() || command.isEmpty())
		{
			continue;
		}

		if (command.at(0) != '*' && command.at(0) != ':')
		{
			command = ":" + command;
		}

		memset(parseCmd, 0, sizeof parseCmd);
        result = GetFunctionCode(command);

		if (__DEBUG)
		{
            printf("cmd[%d] on client[%d] = %s, len = %d, funcCode = %#010X\r\n", i, socketIndex, cmdList.at(i).trimmed().toStdString().c_str(), cmdList.at(i).length(), result.cmdCode);
            fflush(stdout);
        }
        ///////////////////////////////////command process//////////////////////////////////////
		switch (result.cmdCode)
		{

        case SCPI_CMD_STANDARD_IDN_QUERY:
        {
            exeResult = __SCPI_RETURN;
            returnString = VERSION;
        }
        break;
        /* （2）心跳设置
         * 命令：Heartbeat
        */
        case SCPI_CMD_STANDARD_HEART_BEAT:
        {
            QStringList list = result.value.trimmed().toUpper().split(',');
//            if (!guidIslegal(list.at(0))) return;
            returnString.append(":Heartbeat,GUID="+sysData.guidClient+VERSION+"\n");
            exeResult = __SCPI_RETURN;
        }break;
        case RMTP_CMD_SERVER_GET_IP:
        {
            exeResult = __SCPI_RETURN;
            returnString = QString(sysData.gt_rmtp.HostIp);
        }break;
        case RMTP_CMD_SERVER_GET_PORT:
        {
            returnString =  QString::number(sysData.gt_rmtp.HostPort);
            exeResult = __SCPI_RETURN;
        }break;

        case RMTP_CMD_CONNECT_SET_INTERVAL:
        {
            sysData.gt_rmtp.interval = result.value.trimmed().toInt();
            returnString = "OK";
            exeResult = __SCPI_RETURN;
        }break;
        case RMTP_CMD_CONNECT_GET_INTERVAL:
        {
            returnString = QString::number(sysData.gt_rmtp.interval);
            exeResult = __SCPI_RETURN;
        }break;
        case RMTP_CMD_SENSOR_GET_INFORMATION:
        {
            returnString = "imax6ull get sound data";
            exeResult = __SCPI_RETURN;
        }break;
        /*
         * （6）采集板音频数据 采集指令
         * : RecordAudio =On(回车) 开始采集音频数据。
         * : RecordAudio =Off(回车) 立即停止采集音频数据。
         */
        case RMTP_CMD_GET_SOUND_DATA:
        {
            QString segStr = result.value.trimmed().toUpper();
            QStringList sl = segStr.split(',');
            if (!guidIslegal(sl.at(1))) return;
            sl.removeAt(1);
            QString str = "";
            if (sl.count() > 0) {
                QString isOpen = sl.at(0).trimmed().toUpper();
                if (isOpen == "ON")
                    system("/app_qt5/line_in_config.sh");
            }

            if (sl.count() == 1)
            {
                str = sl.at(0).trimmed().toUpper();
                if (str == "ON") {
                    getAlsaData->sdc.enable_recorde = true;
                    sysData.checkStatus.audio = "OK";
                    readData->start();
                    emit emitSendData2Pc();
                }
                else if (str == "OFF") {
                    getAlsaData->sdc.enable_recorde = false;
                    sysData.checkStatus.audio = "OFF";
                    readData->exit();
                }
            }

//            returnString.append(":RecordAudio ="+sysData.checkStatus.audio+"guid="+sysData.guidClient+"\n");
            exeResult = __SCPI_SUCCESS;
        }break;
        /* （1）-1时间设置
         * :SetDateTime=1600675114000 ,GUID=…(回车)
        */
        case SYSTEM_SET_DATA_TIME:
        {
            QStringList list = result.value.trimmed().split(',');
            if (!guidIslegal(list.at(1)))return;
            list.removeAt(1);

            QString timeStamp = list.at(0);
            if (timeStamp.count() != 13) {
                exeResult = __SCPI_FAILED;
            }
            else {
                int timeT = timeStamp.mid(0,10).toInt();
                QDateTime dateTime = QDateTime::fromTime_t(timeT);
                QString str = dateTime.toString("yyyy.MM.dd-hh:mm:ss");
                QString date = "date "+str;
                system(date.toStdString().c_str());
                system("hwclock -w");
            }
            exeResult = __SCPI_SUCCESS;
        }break;
        /* （1）-2获取时间
         * 询问:SetDateTime？ GUID=…;
        */
        case SYSTEM_GET_DATA_TIME:
        {
            if (!guidIslegal(result.value))return;
            exeResult = __SCPI_RETURN;
            QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
            returnString.append(":SetDateTime="+timestamp+"\n");
        }break;
        /* heart beat */
        case SYSTEM_HEART_BEAT:
        {

        }break;
            /* 检查系统状态 */
        case SYSTEM_CHRECK_STATE:
        {


        }break;
        /* (4) 采集板程序重启，需要重新执行脚本 */
        case SYSTEM_RESTART_APP:
        {
            QStringList list = result.value.trimmed().split(',');
            if (!guidIslegal(list.at(0))) return;
            QCoreApplication::exit(0);
            exeResult = __SCPI_RETURN;
//            returnString.append(": Restart ,GUID="+sysData.guidClient+"\n");

        }break;

//        case RMTP_CMD_COLLECT_RADIO_STATUS:
//        {
//            exeResult = __SCPI_SUCCESS;
//            sysData.checkStatus.collectTimeState->start(3000);
//        }break;
        //（3）采集板的自检/状态上报
        case SYSTEM_CHECK_STATUS:
        {
            QStringList list = result.value.trimmed().split(',');
            if (!guidIslegal(list.at(1))) return;
            list.removeAt(1);
            exeResult = __SCPI_SUCCESS;
            QString str = list.at(0).toUpper();
            qDebug()<<str;
            if (str == "ON")
            {
                sysData.checkStatus.collectTimeState->start(sysData.checkStatus.stausTimeStep*1000);
            }
            else if (str == "OFF")
            {
                sysData.checkStatus.collectTimeState->stop();
            }
        }break;
        //（7）工作参数设置
        // : SetParameter, GUID=…, CollectTimeStep=3, StausTimeStep=3 (回车)
        case RMTP_CMD_SET_PARAMETER:
        {
            QStringList sl = result.value.trimmed().split(',');
            if (!guidIslegal(sl.at(0))) return;
            sl.removeAt(0);
            exeResult = __SCPI_SUCCESS;
            QString str = "";
            if (sl.count() < 2) {
                qDebug()<<"parameters error";
                exeResult = __SCPI_FAILED;
            }
            str = sl.at(0).trimmed();
            QStringList sl1 = str.split('=');
            if (sl1.count() >= 2) {
                str = sl1.at(0).trimmed().toUpper();
                if (str == "COLLECTTIMESTEP"){
                    str = sl1.at(1).trimmed();
                    sysData.checkStatus.collectTimeStep = str.toInt();
                }
            }
            str = sl.at(1).trimmed();
            QStringList sl2 = str.split('=');
            if (sl2.count() >= 2) {
                str = sl2.at(0).trimmed().toUpper();
                if (str == "STAUSTIMESTEP"){
                    str = sl2.at(1).trimmed();
                    sysData.checkStatus.stausTimeStep = str.toInt();
                }
            }
            qDebug("%d,%d",sysData.checkStatus.collectTimeStep,sysData.checkStatus.stausTimeStep);
        }break;
        /*
         * （5）采集板采集数据指令
         *  命令：CollectRadioStatus
         * : RecordAudio =On, GUID=…(回车)
         * : RecordAudio =Off, GUID=…(回车)
        */
        case SYSTEM_COLLECT_RADIO_STATUS:{
            QStringList list = result.value.trimmed().split(',');
            if (list.count() < 2) return;
            if (!guidIslegal(list.at(1))) return;
            list.removeAt(1);
            QString string = list.at(0).toUpper();
            if (string == "ON")
                sysData.collect_radio_data_enable = true;
            else
                sysData.collect_radio_data_enable = false;
            exeResult = __SCPI_RETURN;
            returnString.append(":CollectRadioStatus = "+string+",guid="+sysData.guidClient+"\n");
        }break;
        case SYSTEM_RECORD_AUDIO_ENABLE:{
            QString string;

            exeResult = __SCPI_RETURN;
            returnString.append(":RecordAudio  = "+string+sysData.guidClient+"\n");
        }break;
        case SYSTEM_SET_GUID: {
            QStringList list = result.value.trimmed().toUpper().split('=');
            QString guid = list.at(1);
            if (guid.length() != 36 || list.at(0).toUpper() != "GUID") return;
            sysData.guidClient = guid;
            guid2hex();
            exeResult = __SCPI_SUCCESS;
        }break;
        /* 设置放大器 */
        case SYSTEM_SET_AMPLIFY:{
            QStringList list = result.value.trimmed().split(',');
            if (list.count() < 2) return;
            if (!guidIslegal(list.at(1))) return;
            list.removeAt(1);
            QString arg = list.at(0).toUpper();

            if (arg == "ON"){
                gpio->soundAmplify(ENBALE_AMPLIFY);
            } else if(arg == "OFF") {
                gpio->soundAmplify(DISABLE_AMPLIFY);
            }
            returnString.append("AMPLify "+arg +" OK "+sysData.guidClient+"\n");
            exeResult = __SCPI_RETURN;
        }break;
        /* 设置干扰模式 */
        case SYSTEM_SET_STATE_DISTURB:{
            QString str = result.value.trimmed();

            if (!guidIslegal(str)) return;
            gpio->soundChannelSelect(DISTURB);
            returnString.append("DISTurb OK "+sysData.guidClient+"\n");
            exeResult = __SCPI_RETURN;
        }break;
        default:
            exeResult = __SCPI_UNSUPPORT;
            break;
        }

        /* 返回执行结果 */
		switch (exeResult)
		{
            case __SCPI_RETURN:
                returnScpiResult(returnString);
				break;
			case __SCPI_FAILED:
                returnScpiResult((QString) "ERR");
				break;
			case __SCPI_UNSUPPORT:
                returnScpiResult((QString) "N/A");
				break;
        }
    }
}

/*
 * 返回SCPI结果
*/
void tSysScpi::returnScpiResult(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return;
	}

	QString sendString = value;
    sendString += SCPIEOFCHAR[0];
    sendString += SCPIEOFCHAR[1];

    emit emitWriteData((char *)sendString.toStdString().data(), sendString.length());
}
/**
 * @brief 判断guid是否合法
 *
 * @param string
 * @return true
 * @return false
 */
bool tSysScpi::guidIslegal(QString string)
{
    qDebug()<<string;
    QStringList list = string.trimmed().toUpper().split('=');
    if (list.count() >= 2 && (list.at(0) == "GUID")) {
        /* updata guid */
        QString st = list.at(1).toUpper();
//        if (st == sysData.guidClient || st == sysData.guidRoot)
        if (QString::compare(st,sysData.guidClient,Qt::CaseInsensitive)== 0
                || QString::compare(st,sysData.guidRoot,Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

/**
 * @brief 判断guid是否合法
 *
 * @param array
 * @return true 合法
 * @return false 不合法
 */
bool tSysScpi::guidIslegal(QByteArray array)
{
    bool ret = true;
    int length = 16;

    if (array.length() != length)
        ret = false;

    for (int i = 0; i < length; i++)
    {
        if (array.at(i) == sysData.guidClienthex[i])
            continue;
        else
        {
            ret = false;
            break;
        }
    }

    return ret;
}
