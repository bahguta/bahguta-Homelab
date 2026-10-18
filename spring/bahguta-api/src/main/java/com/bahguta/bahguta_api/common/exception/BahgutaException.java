package com.bahguta.bahguta_api.common.exception;



public abstract class BahgutaException extends Exception {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 5813748683304783012L;

	public BahgutaException(String msg, Throwable cause) {
		super(msg, cause);
	}
	
	public BahgutaException(String msg) {
		super(msg, null);
	}

}
