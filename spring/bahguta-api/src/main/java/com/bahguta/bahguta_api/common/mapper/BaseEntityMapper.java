package com.bahguta.bahguta_api.common.mapper;

import java.util.List;
import org.mapstruct.Mapper;
import com.bahguta.bahguta_api.common.dto.BaseEntityDto;
import com.bahguta.bahguta_api.common.entity.BaseEntity;

@Mapper(componentModel = "spring")
public interface BaseEntityMapper {

	BaseEntityDto toDto(BaseEntity entity);
		
	List<BaseEntityDto> toDtoList(List<BaseEntity> entities);
		
}
