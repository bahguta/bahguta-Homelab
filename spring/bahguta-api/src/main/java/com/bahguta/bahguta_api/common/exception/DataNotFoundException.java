package com.bahguta.bahguta_api.common.exception;

public class DataNotFoundException extends BahgutaException{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 7441327991454676895L;

	public DataNotFoundException(String msg, Throwable causa) {
		super(msg, causa);
	}

	public DataNotFoundException(String msg) {
		super(msg, null);
	}
}
