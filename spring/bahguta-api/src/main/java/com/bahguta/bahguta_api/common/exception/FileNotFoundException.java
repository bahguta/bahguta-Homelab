package com.bahguta.bahguta_api.common.exception;

public class FileNotFoundException extends BahgutaException {

	/**
	 * 
	 */
	private static final long serialVersionUID = 666790065544984933L;

	public FileNotFoundException(String msg, Throwable cause) {
		super(msg, cause);
	}
	
	public FileNotFoundException(String msg) {
		super(msg);
	}


}
