/*
    DABlin - capital DAB experience
    Copyright (C) 2016-2018 Stefan Pöschel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mot_manager.h"


// --- MOTEntity -----------------------------------------------------------------
void MOTEntity::AddSeg(int seg_number, bool last_seg, const uint8_t* data, size_t len) {
                                                                qDebug()<<"MOTEntity::AddSeg start";
	if(last_seg)
		last_seg_number = seg_number;

	if(segs.find(seg_number) != segs.end())
		return;

	// copy data
	segs[seg_number] = seg_t(len);
                                                                //qDebug()<<"memcpy addseg Entity"<<memcpy(&segs[seg_number][0], data, len);
	memcpy(&segs[seg_number][0], data, len);
	size += len;
}

bool MOTEntity::IsFinished() {
                                                                qDebug()<<"MOTEntity::IsFinished start";
                                                                //qDebug()<<"last_seg_number"<<last_seg_number;
	if(last_seg_number == -1)
		return false;

	// check if all segments are available
	for(int i = 0; i <= last_seg_number; i++)
                                                                //qDebug()<<"segs.find(i)"<<segs;
		if(segs.find(i) == segs.end())
			return false;
                                                                //qDebug()<<"return true MOTEntity::IsFinished() for reached";
	return true;
}

std::vector<uint8_t> MOTEntity::GetData() {
                                                                qDebug()<<"MOTEntity::GetData start";
	std::vector<uint8_t> result(size);
    size_t offset = 0;

	// concatenate all segments
	for(int i = 0; i <= last_seg_number; i++) {
		seg_t& seg = segs[i];
		memcpy(&result[offset], &seg[0], seg.size());
		offset += seg.size();
	}
                                                                //qDebug()<<"result"<<result;
	return result;
}


// --- MOTObject -----------------------------------------------------------------
void MOTObject::AddSeg(bool dg_type_header, int seg_number, bool last_seg, const uint8_t* data, size_t len) {
                                                                qDebug()<<"MOTObject::AddSeg start";
	(dg_type_header ? header : body).AddSeg(seg_number, last_seg, data, len);
}

bool MOTObject::ParseCheckHeader(MOT_FILE& target_file) {
                                                                qDebug()<<"MOTObject::ParseCheckHeader start";
	MOT_FILE file = target_file;
    std::vector<uint8_t> data = header.GetData();               //qDebug()<<"data in MOTObject ParseCheckHeader"<<data;

	// parse/check header core
	if(data.size() < 7)
		return false;

    size_t body_size = (data[0] << 20) | (data[1] << 12) | (data[2] << 4) | (data[3] >> 4); //qDebug()<<"body_size"<<body_size;
    size_t header_size = ((data[3] & 0x0F) << 9) | (data[4] << 1) | (data[5] >> 7);         //qDebug()<<"header_size"<<header_size;
    int content_type = (data[5] & 0x7F) >> 1;                                               //qDebug()<<"content_type"<<content_type;
    int content_sub_type = ((data[5] & 0x01) << 8) | data[6];                               //qDebug()<<"conten_sub_type"<<content_sub_type;

    //type 2 == image, 5 == transport
    //subtype == 0 ? "GIF" : subtype == 1 ? "JPEG" : subtype == 2 ? "BMP" : "PNG"


//	fprintf(stderr, "body_size: %5zu, header_size: %3zu, content_type: 0x%02X, content_sub_type: 0x%03X\n",
//			body_size, header_size, content_type, content_sub_type);

	if(header_size != header.GetSize())
		return false;
                                                                //qDebug()<<"header_size != header.GetSize() passed";
	bool header_update =
			content_type == MOT_FILE::CONTENT_TYPE_MOT_TRANSPORT &&
			content_sub_type == MOT_FILE::CONTENT_SUB_TYPE_HEADER_UPDATE;

                                                                //qDebug()<<"content_type"<<content_type;
                                                                //qDebug()<<"content_sub_type"<<content_sub_type;

	// abort, if neither none nor both conditions (header received/update) apply
	if(header_received != header_update)
		return false;
                                                                //qDebug()<<"header_received != header_update passed";
	if(!header_update) {
		// store core info
        file.body_size = body_size;                             //qDebug()<<"file.body_size"<<file.body_size;
        file.content_type = content_type;                       //qDebug()<<"file.conten_type"<<file.content_type;
        file.content_sub_type = content_sub_type;               //qDebug()<<"file.content_sub_type"<<file.content_sub_type;
    }

    std::string old_content_name = file.content_name;
	std::string new_content_name;

	// parse/check header extension
	for(size_t offset = 7; offset < data.size();) {
        int pli = data[offset] >> 6;                            //qDebug()<<"pli"<<pli;
        int param_id = data[offset] & 0x3F;                     //qDebug()<<"param_id"<<param_id;
        offset++;

		// get parameter len
		size_t data_len;
		switch(pli) {
		case 0b00:
			data_len = 0;
			break;
		case 0b01:
			data_len = 1;
			break;
		case 0b10:
			data_len = 4;
			break;
		case 0b11:
			if(offset >= data.size())
				return false;
			bool ext = data[offset] & 0x80;
			data_len = data[offset] & 0x7F;
			offset++;

			if(ext) {
				if(offset >= data.size())
					return false;
				data_len = (data_len << 8) + data[offset];
				offset++;
			}
			break;
		}

		if(offset + data_len - 1 >= data.size())
			return false;

		// process parameter
		switch(param_id) {
		case 0x05:	// TriggerTime
			if(data_len < 4)
				return false;
			// TODO: not only distinguish between Now or not
			file.trigger_time_now = !(data[offset] & 0x80);
            fprintf(stderr, "TriggerTime: %s\n", file.trigger_time_now ? "Now" : "(not Now)");
			break;
		case 0x0C:	// ContentName
			if(data_len == 0)
				return false;
			//file.content_name = CharsetTools::ConvertTextToUTF8(&data[offset + 1], data_len - 1, data[offset] >> 4, true, &file.content_name_charset);
            file.content_name = toUtf8StringUsingCharset ( (const char *)&data[offset + 1], (CharacterSet) (data[offset] >> 4), data_len - 1);
			new_content_name = file.content_name;
            fprintf(stderr, "ContentName: '%s'\n", file.content_name.c_str());
			break;
		case 0x26:	// CategoryTitle
			file.category_title = std::string((char*) &data[offset], data_len);	// already UTF-8
            fprintf(stderr, "CategoryTitle: '%s'\n", file.category_title.c_str());
			break;
		case 0x27:	// ClickThroughURL
			file.click_through_url = std::string((char*) &data[offset], data_len);	// already UTF-8
            fprintf(stderr, "ClickThroughURL: '%s'\n", file.click_through_url.c_str());
			break;
		}
		offset += data_len;
	}

	if(!header_update) {
		// ensure actual header is processed only once
		header_received = true;
	} else {
		// ensure matching content name
		if(new_content_name != old_content_name)
			return false;
	}

	target_file = file;
                                                                qDebug()<<"all done and passed";
	return true;
}

bool MOTObject::IsToBeShown() {
                                                                qDebug()<<"MOTObject::IsToBeShown start";
	// abort, if already shown
	if(shown)
		return false;

	// try to process finished header
    if(header.IsFinished()) {                                   qDebug()<<"MOTObject::IsToBeShown() IsFinished check Header start";
		// parse/check MOT header
		bool result = ParseCheckHeader(result_file);
		header.Reset();	// allow for header updates
		if(!result)
			return false;
    }                                                           qDebug()<<"MOTObject::IsToBeShown() IsFinished check Header passed";

	// abort, if incomplete/not yet triggered
    if(!header_received)
		return false;
                                                                qDebug()<<"MOTObject::IsToBeShown() IsFinished check Body start";
	if(!body.IsFinished() || result_file.body_size != body.GetSize())
		return false;
	if(!result_file.trigger_time_now)
		return false;

	// add body data
	result_file.data = body.GetData();

	shown = true;
                                                                qDebug()<<"MOTObject::IsToBeShown() all passed and ready for pic";
	return true;
}


// --- MOTManager -----------------------------------------------------------------
MOTManager::MOTManager() {
                                                                qDebug()<<"MOTManager::MOTManager start";
	Reset();
}

void MOTManager::Reset() {
                                                                qDebug()<<"MOTManager::Reset start";
	object = MOTObject();
	current_transport_id = -1;
}

bool MOTManager::ParseCheckDataGroupHeader(const std::vector<uint8_t>& dg, size_t& offset, int& dg_type) {
                                                                qDebug()<<"MOTManager::ParseCheckDataGroupHeader start";
                                                                //qDebug()<<dg;
                                                                //qDebug()<<"offset"<<offset;
                                                                //qDebug()<<"dg.size"<<dg.size();
                                                                //qDebug()<<"offset+2"<<(offset + 2);
	// parse/check Data Group header
	if(dg.size() < (offset + 2))
		return false;

    bool extension_flag = dg[offset] & 0x80;                    //qDebug()<<extension_flag << "dg[offset]" << dg[offset];
    bool crc_flag = dg[offset] & 0x40;                          //qDebug()<<crc_flag << "dg[offset]" << dg[offset];
    bool segment_flag = dg[offset] & 0x20;                      //qDebug()<<segment_flag << "dg[offset]" << dg[offset];
    bool user_access_flag = dg[offset] & 0x10;                  //qDebug()<<user_access_flag << "dg[offset]" << dg[offset];
    dg_type = dg[offset] & 0x0F;                                //qDebug()<<"dg_type = dg[offset] & 0x0F"<<dg_type<< "dg[offset]" << dg[offset];
    offset += 2 + (extension_flag ? 2 : 0);                     //qDebug()<<"offset"<<offset;

    if(!crc_flag)
        return false;                                   //printf("1");//qDebug()<<"crc_flag";
    if(!segment_flag)
        return false;                                   //printf("2");//qDebug()<<"segment_flag";
    if(!user_access_flag)
        return false;                                   //printf("3");//qDebug()<<"user_acc";
	if(dg_type != 3 && dg_type != 4)	// only accept MOT header/body
        return false;                                   //printf("4");//qDebug()<<"4te stufe";

	return true;
}

bool MOTManager::ParseCheckSessionHeader(const std::vector<uint8_t>& dg, size_t& offset, bool& last_seg, int& seg_number, int& transport_id) {
                                                                qDebug()<<"MOTManager::ParseCheckSessionHeader start";
                                                                //qDebug()<<"offset overload"<<offset;
                                                                //qDebug()<<"dg"<<dg;
    // parse/check session header
	if(dg.size() < (offset + 3))
		return false;

    last_seg = dg[offset] & 0x80;                               //qDebug()<<"last_seg"<<last_seg;
    seg_number = ((dg[offset] & 0x7F) << 8) | dg[offset + 1];   //qDebug()<<"seg_number"<<seg_number;  //qDebug()<<"seg_number"<<(seg_number > 0 ? (seg_number / 4096) : 0);
    //seg_number = (seg_number > 0 ? (seg_number / 4096) : 0);
    bool transport_id_flag = dg[offset + 2] & 0x10;             //qDebug()<<"transport_id_flag"<<transport_id_flag;
    size_t len_indicator = dg[offset + 2] & 0x0F;               //qDebug()<<"len_indicator"<<len_indicator;
    offset += 3;                                                //qDebug()<<"offset"<<offset;

	if(!transport_id_flag)
		return false;
	if(len_indicator < 2)
		return false;

	// handle transport ID
	if(dg.size() < (offset + len_indicator))
		return false;

    transport_id = (dg[offset] << 8) | dg[offset + 1];          //qDebug()<<"transport_id"<<transport_id;
	offset += len_indicator;

	return true;
}

bool MOTManager::ParseCheckSegmentationHeader(const std::vector<uint8_t>& dg, size_t& offset, size_t& seg_size) {
                                                                qDebug()<<"MOTManager::ParseCheckSegmentationHeader start";
                                                                //qDebug()<<"offset overload2"<<offset;
    // parse/check segmentation header (MOT)
	if(dg.size() < (offset + 2))
		return false;

    seg_size = ((dg[offset] & 0x1F) << 8) | dg[offset + 1];     //qDebug()<<"seg_size"<<seg_size;
	offset += 2;
                                                                //qDebug()<<"offset"<<offset;
                                                                //qDebug()<<"calccrc"<<(seg_size != dg.size() - offset - CalcCRC::CRCLen + 1);
                                                                //qDebug()<<"dg.size()"<<dg.size();
                                                                //qDebug()<<"offset crc"<<offset;
                                                                //qDebug()<<"CRCLen"<<CalcCRC::CRCLen;



	// compare announced/actual segment size
    if(seg_size != dg.size() - offset - CalcCRC::CRCLen)    //qDebug()<<"calccrc"<<(seg_size != dg.size() - offset - CalcCRC::CRCLen);
		return false;

	return true;
}

bool MOTManager::HandleMOTDataGroup(const std::vector<uint8_t>& dg) {
                                                                qDebug()<<"MOTManager::HandleMOTDataGroup start";
    size_t offset = 0;

	// parse/check headers
	int dg_type;
	bool last_seg;
	int seg_number;
	int transport_id;
	size_t seg_size;

//    qDebug()<<"dg_type"<<dg_type;
//    qDebug()<<"last_seg"<<last_seg;
//    qDebug()<<"seg_number"<<seg_number;
//    qDebug()<<"transport_id"<<transport_id;
//    qDebug()<<"seg_size"<<seg_size;



    if(!ParseCheckDataGroupHeader(dg, offset, dg_type))         //qDebug()<<"PCDGH"<<ParseCheckDataGroupHeader(dg, offset, dg_type);
        return false;
	if(!ParseCheckSessionHeader(dg, offset, last_seg, seg_number, transport_id))
		return false;
	if(!ParseCheckSegmentationHeader(dg, offset, seg_size))
		return false;

                                                                qDebug()<<"start after checks---------------------------------------------";
	// add segment to MOT object (reset if necessary)
	if(current_transport_id != transport_id) {
		current_transport_id = transport_id;
		object = MOTObject();
	}
                                                                //qDebug()<<"seg_number"<<seg_number;
                                                                //qDebug()<<"last_seg"<<last_seg;
                                                                //qDebug()<<"dg[offset]"<<dg[offset];
                                                                //qDebug()<<"seg_size"<<seg_size;

	object.AddSeg(dg_type == 3, seg_number, last_seg, &dg[offset], seg_size);

	// check if object shall be shown
	bool display = object.IsToBeShown();
//	fprintf(stderr, "dg_type: %d, seg_number: %2d%s, transport_id: %5d, size: %4zu; display: %s\n",
//			dg_type, seg_number, last_seg ? " (LAST)" : "", transport_id, seg_size, display ? "true" : "false");

	// if object shall be shown, update it
	return display;
}
